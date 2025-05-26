// =============================================================================
// Rebol/Ultraleap extension commands
// =============================================================================


#include "ultraleap-rebol-extension.h"
#include <stdio.h>
#include <stdlib.h> // malloc
#include <math.h>   // fmin, fmax
#include "LeapConnection.h"

reb_leap_connection *CurrentConnection = NULL;
REBCNT LeapDataStructId = 0;
REBOL_LEAP_DATA *CurrentConnectionData = NULL;

#define COMMAND int

#define FRM_IS_HANDLE(n, t)   (RXA_TYPE(frm,n) == RXT_HANDLE && RXA_HANDLE_TYPE(frm, n) == t)
#define FRM_IS_STRUCT(n)      (RXA_TYPE(frm,n) == RXT_STRUCT)
#define ARG_Double(n)         RXA_DEC64(frm,n)
#define ARG_Float(n)          (float)RXA_DEC64(frm,n)
#define ARG_Int32(n)          RXA_INT32(frm,n)
#define ARG_Handle_Series(n)  RXA_HANDLE_CONTEXT(frm, n)->series;

#define RETURN_HANDLE(hob)                   \
	RXA_HANDLE(frm, 1)       = hob;          \
	RXA_HANDLE_TYPE(frm, 1)  = hob->sym;     \
	RXA_HANDLE_FLAGS(frm, 1) = hob->flags;   \
	RXA_TYPE(frm, 1) = RXT_HANDLE;           \
	return RXR_VALUE

#define APPEND_STRING(str, ...) \
	len = snprintf(NULL,0,__VA_ARGS__);\
	if (len > SERIES_REST(str)-SERIES_LEN(str)) {\
		RL_EXPAND_SERIES(str, SERIES_TAIL(str), len);\
		SERIES_TAIL(str) -= len;\
	}\
	len = snprintf( \
		SERIES_TEXT(str)+SERIES_TAIL(str),\
		SERIES_REST(str)-SERIES_TAIL(str),\
		__VA_ARGS__\
	);\
	SERIES_TAIL(str) += len;

#define RETURN_ERROR(err)  do {RXA_SERIES(frm, 1) = (void*)err; return RXR_ERROR;} while(0)


int Common_mold(REBHOB *hob, REBSER *str) {
	int len;
	if (!str) return 0;
	SERIES_TAIL(str) = 0;
	APPEND_STRING(str, "0#%lx", (unsigned long)(uintptr_t)hob->data);
	return len;
}


int UltraleapHandle_free(void* hndl) {
	RXIARG  arg;
	REBSER *blk;
	REBHOB *hob;

	if (!hndl) return 0;
	hob = (REBHOB *)hndl;

	printf("release engine: %p is referenced: %i\n", hob->data, IS_MARK_HOB(hob) != 0);
	UNMARK_HOB(hob);
	blk = hob->series;
	if (blk) {
		int i = blk->tail;
		while (i-->0) {
			if (RXT_HANDLE == RL_GET_VALUE(blk, i, &arg)) {
				RL_FREE_HANDLE_CONTEXT(arg.handle.ptr);
			}
		}
		RESET_SERIES(blk);
		hob->series = NULL;
	}
//	engine = (my_engine*)hob->data;
//	ma_engine_uninit(&engine->engine);
//	ma_device_uninit(&engine->device);
	return 0;
}
int UltraleapHandle_get_path(REBHOB *hob, REBCNT word, REBCNT *type, RXIARG *arg) {
	reb_leap_connection *ctx = (reb_leap_connection*)hob->data;
	word = RL_FIND_WORD(arg_words, word);
	arg->uint64 = 0;
	switch (word) {
	case W_ARG_DEVICE_ID:
		*type = RXT_INTEGER;
		if(ctx->deviceInfo) {
			arg->uint64 = (u64)ctx->deviceInfo->pid;
		}
		break;
	case W_ARG_STATUS:
		*type = RXT_INTEGER;
		if(ctx->deviceInfo) {
			arg->uint64 = (u64)ctx->deviceInfo->status;
		}
		break;
//	case W_ARG_RESOURCES:
//		*type = RXT_BLOCK;
//		arg->series = hob->series;
//		arg->index = 0;
//		break;
	default:
		return PE_BAD_SELECT;	
	}
	return PE_USE;
}
int UltraleapHandle_set_path(REBHOB *hob, REBCNT word, REBCNT *type, RXIARG *arg) {
//	ma_engine* engine = (ma_engine*)hob->data;
	word = RL_FIND_WORD(arg_words, word);
	switch (word) {
//	case W_ARG_VOLUME:
//		switch (*type) {
//		case RXT_DECIMAL:
//		case RXT_PERCENT:
//			ma_engine_set_volume(engine, arg->dec64);
//			break;
//		case RXT_INTEGER:
//			ma_engine_set_volume(engine, (float)arg->int64);
//			break;
//		default: 
//			return PE_BAD_SET_TYPE;
//		}
//		break;
	default:
		return PE_BAD_SET;	
	}
	return PE_OK;
}



/** Callback for when the connection opens. */
static void OnConnect(void){
	debug_print("Connected.\n");
}

/** Callback for when a device is found. */
static void OnDevice(const LEAP_DEVICE_INFO *props){
	debug_print("Found device %s.\n", props->serial);
	if (CurrentConnection) {
		CurrentConnection->deviceInfo = props;
	}
}

static void OnPolicy(const uint32_t current_policies) {
	debug_print("current_policies: %u.\n", current_policies);
	if (CurrentConnectionData) {
		CurrentConnectionData->policy = current_policies;
	}
}

static void OnTracking(const LEAP_TRACKING_MODE_EVENT *mode_event) {
	debug_print("current_tracking_mode: %u.\n", mode_event->current_tracking_mode);
	if (CurrentConnectionData) {
		CurrentConnectionData->tracking = mode_event->current_tracking_mode;
	}
}

/** Callback for when a frame of tracking data is available. */
static void OnFrame(const LEAP_TRACKING_EVENT *frame){
//	if (frame->info.frame_id % 60 == 0)
//		debug_print("Frame %lli with %i hands.\n", (long long int)frame->info.frame_id, frame->nHands);
	if (!CurrentConnectionData) return;
	CurrentConnectionData->hands = frame->nHands;
	CurrentConnectionData->framerate = frame->framerate;
	CurrentConnectionData->timestamp = frame->info.timestamp;

	for(uint32_t h = 0; h < frame->nHands; h++){
		LEAP_HAND* hand = &frame->pHands[h];
		if (hand->type == eLeapHandType_Left) {
			CurrentConnectionData->left = *hand;
		} else {
			CurrentConnectionData->right = *hand;
		}
	}
}

static void OnImage(const LEAP_IMAGE_EVENT *image){
	debug_print("Image %lli  => Left: %d x %d (bpp=%d), Right: %d x %d (bpp=%d)\n",
		(long long int)image->info.frame_id,
		image->image[0].properties.width,image->image[0].properties.height,image->image[0].properties.bpp*8,
		image->image[1].properties.width,image->image[1].properties.height,image->image[1].properties.bpp*8);
}






COMMAND cmd__init(RXIFRM *frm, void *ctx) {
	LeapDataStructId = RXA_STRUCT_ID(frm,1);
	arg_words  = RL_MAP_WORDS(RXA_SERIES(frm,2));
	type_words = RL_MAP_WORDS(RXA_SERIES(frm,3));

	CurrentConnectionData = (REBOL_LEAP_DATA*)RXA_STRUCT_BIN(frm,1);

	//printf("Data struct id: %u\n", LeapDataStructId);
	// custom initialization may be done here...

	return RXR_TRUE;
}

COMMAND cmd_connect(RXIFRM *frm, void *rctx) {
	REBHOB *hob = NULL;
	reb_leap_connection *ctx;

	if (CurrentConnection) RETURN_ERROR("Already connected!");

	hob = RL_MAKE_HANDLE_CONTEXT(Handle_UltraleapHandle);
	if (hob == NULL) return RXR_NONE;

	ctx = (reb_leap_connection*)hob->data;
	CurrentConnection = ctx;

	//CurrentConnection->data = (REBOL_LEAP_DATA*)RXA_STRUCT_BIN(frm, 1);

	//Set callback function pointers
	ConnectionCallbacks.on_connection          = &OnConnect;
	ConnectionCallbacks.on_device_found        = &OnDevice;
	ConnectionCallbacks.on_frame               = &OnFrame;
	ConnectionCallbacks.on_image               = &OnImage;
	ConnectionCallbacks.on_policy              = &OnPolicy;
	ConnectionCallbacks.on_tracking_mode       = &OnTracking;

	ctx->connectionHandle = OpenConnection();
	if (ctx->connectionHandle == NULL) {
		RETURN_ERROR("Failed to OpenConnection!");
	}

	LeapSetPolicyFlags(*ctx->connectionHandle, eLeapPolicyFlag_MapPoints | eLeapPolicyFlag_AllowPauseResume, 0);
	LeapSetTrackingMode(*ctx->connectionHandle, eLeapTrackingMode_Desktop);
	RETURN_HANDLE(hob);
}

COMMAND cmd_pause(RXIFRM *frm, void *rctx) {
	if (!CurrentConnection) return RXR_FALSE;
	eLeapRS rs = LeapSetPause(*CurrentConnection->connectionHandle, TRUE);
	if (rs != eLeapRS_Success){
		const char* err = ResultString(rs);
		RETURN_ERROR(err);
	}
	return RXR_TRUE;
}

COMMAND cmd_resume(RXIFRM *frm, void *rctx) {
	if (!CurrentConnection) return RXR_FALSE;
	eLeapRS rs = LeapSetPause(*CurrentConnection->connectionHandle, FALSE);
	if (rs != eLeapRS_Success){
		const char* err = ResultString(rs);
		RETURN_ERROR(err);
	}
	return RXR_TRUE;
}

COMMAND cmd_policy(RXIFRM *frm, void *rctx) {
	if (!CurrentConnection) return RXR_FALSE;
	uint64_t set = RXA_UINT64(frm,1);
	uint64_t clr = RXA_UINT64(frm,2);
	eLeapRS rs = LeapSetPolicyFlags(*CurrentConnection->connectionHandle, set, clr);
	if (rs != eLeapRS_Success){
		const char* err = ResultString(rs);
		RETURN_ERROR(err);
	}
	return RXR_TRUE;
}

COMMAND cmd_tracking(RXIFRM *frm, void *rctx) {
	if (!CurrentConnection) return RXR_FALSE;
	uint64_t mode = RXA_UINT64(frm,1);
	eLeapRS rs = LeapSetTrackingMode(*CurrentConnection->connectionHandle, (eLeapTrackingMode)mode);
	if (rs != eLeapRS_Success){
		const char* err = ResultString(rs);
		RETURN_ERROR(err);
	}
	return RXR_TRUE;
}