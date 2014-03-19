

using namespace System;
using namespace System::Windows::Forms;

#define HED


public ref class DSComment{
	
	public:
		
		

		DWORD dwRegister;
		bool bCaptureDevConnect;

		String^ strCaptureDevName;

		BITMAPINFOHEADER *pbmi;
		
		IGraphBuilder *pGraphManager;
		ICaptureGraphBuilder2 *pCapture;
		ISampleGrabber *pGrabber;
		IVideoWindow  *pVW;
		IMediaControl *pMC;
		IMediaControl *pCaptureMC;
		IMediaEventEx *pME;
		IBaseFilter *pCaptureBaseFilter;
		IBaseFilter *pGrabberBaseFilter;
		IBaseFilter *pNullRender;
		IPin *pGrabberOutPin;

		

	HRESULT PreviewVideo();

	

	
};
