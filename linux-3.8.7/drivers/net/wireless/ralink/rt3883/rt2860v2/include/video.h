#ifdef VIDEO_TURBINE_SUPPORT
extern AP_VIDEO_STRUCT GLOBAL_AP_VIDEO_CONFIG;

VOID VideoModeUpdate(IN PRTMP_ADAPTER pAd);
VOID VideoModeDynamicTune(IN PRTMP_ADAPTER pAd);
UINT32 GetAsicDefaultRetry(IN PRTMP_ADAPTER pAd);
UCHAR GetAsicDefaultTxBA(IN PRTMP_ADAPTER pAd);
UINT32 GetAsicVideoRetry(IN PRTMP_ADAPTER pAd);
UCHAR GetAsicVideoTxBA(IN PRTMP_ADAPTER pAd);
VOID VideoConfigInit(IN PRTMP_ADAPTER pAd);
VOID VideoTurbineDynamicTune(IN PRTMP_ADAPTER pAd);
#endif // VIDEO_TURBINE_SUPPORT //

