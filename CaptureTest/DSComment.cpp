#include "stdafx.h"
#include "DSComment.h"

using namespace System;
using namespace System::Windows::Forms;

#define HED
		

		DWORD dwRegister;
		bool bCaptureDevConnect;

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

		HRESULT DSComment::PreviewVideo(){

			HRESULT hr;

			cli::pin_ptr<IGraphBuilder *> ppGraphManager = &pGraphManager;
				hr = CoCreateInstance (
				CLSID_FilterGraph, 
				NULL,
				CLSCTX_INPROC_SERVER,   //for add grephedit
				IID_IGraphBuilder, 
				(void **) ppGraphManager
				);
			if (FAILED(hr)){
				MessageBox::Show("Initial IFilterGraph2 failed!");
				return E_NOINTERFACE;
			}

			ULONG cFetched;
			const char* CAPTUREDEVNAME;
			

	#ifdef HED
			CAPTUREDEVNAME = "USB  ”∆µ…Ë±∏";
	#else
			CAPTUREDEVNAME = "Microsoft LifeCam HD-3000";
	#endif

			cli::pin_ptr<IBaseFilter *> ppCaptureBaseFilter = &pCaptureBaseFilter;
		   
			// Create the system device enumerator
			ICreateDevEnum *pDevEnum =NULL;
			
			hr = CoCreateInstance (
				CLSID_SystemDeviceEnum, 
				NULL, 
				CLSCTX_INPROC,
				IID_ICreateDevEnum, 
				(void **) &pDevEnum
				);
			if (FAILED(hr)){
				MessageBox::Show("Couldn't create system enumerator!");
				return E_NOINTERFACE;
			}

			// Create an enumerator for the video capture devices
			IEnumMoniker *pClassEnum = NULL;

			hr = pDevEnum->CreateClassEnumerator(
				CLSID_VideoInputDeviceCategory, 
				&pClassEnum, 
				0
				);

			if (FAILED(hr)){
				MessageBox::Show("Couldn't create class enumerator!");
				return E_NOINTERFACE;
			}

			// If there are no enumerators for the requested type, then 
			// CreateClassEnumerator will succeed, but pClassEnum will be NULL.
			if (pClassEnum == NULL){
				MessageBox::Show("No video capture device was detected.\r\n\r\n	  \
						This sample requires a video capture device, such as a USB WebCam,\r\n \
						to be installed and working properly.  The sample will now close. \
						No Video Capture Hardware");
				return E_FAIL;
			}

			IMoniker *pMoniker = NULL;

			// Use the first video capture device on the device list.
			// Note that if the Next() call succeeds but there are no monikers,
			// it will return S_FALSE (which is not a failure).  Therefore, we
			// check that the return code is S_OK instead of using SUCCEEDED() macro.
			if (S_OK == (pClassEnum->Next (1, &pMoniker, &cFetched))){

				// Bind Moniker to a filter object
				hr = pMoniker->BindToObject(0,0,IID_IBaseFilter, (void**)ppCaptureBaseFilter);
				
				IPropertyBag *pPropBag = NULL;
				hr = pMoniker->BindToStorage(0, 0, IID_IPropertyBag, (void **)&pPropBag);
				if (FAILED(hr)){
					pMoniker->Release();
					return E_NOINTERFACE;  
				} 
				
				VARIANT var;
				VariantInit(&var);

				// Get description or friendly name.
				hr = pPropBag->Read(L"Description", &var, 0);
				if (FAILED(hr)){
					hr = pPropBag->Read(L"FriendlyName", &var, 0);
				}

				if (SUCCEEDED(hr)){
					String^ strCaptureDevName = String(var.bstrVal).ToString();
					if(!strCaptureDevName->CompareTo(String(CAPTUREDEVNAME).ToString())){
						hr = S_OK;
					}else{
						hr = E_FAIL;
						return hr;
					}
				}
				pPropBag->Release();
			}else{
				MessageBox::Show("Unable to access video capture device!");   
				return E_FAIL;
			}

			
			pMoniker->Release();
			pClassEnum->Release();
			pDevEnum->Release();
			// Copy the found filter pointer to the output parameter.
			// Do NOT Release() the reference, since it will still be used
			// by the calling function.

			pGraphManager->AddFilter(pCaptureBaseFilter, L"eID Capture Device");

			cli::pin_ptr<IBaseFilter *> ppGrabberBaseFilter = &pGrabberBaseFilter;
		
			hr = CoCreateInstance(
				CLSID_SampleGrabber, 
				NULL, 
				CLSCTX_INPROC_SERVER,
				IID_IBaseFilter, 
				(void**)ppGrabberBaseFilter
				);
			

			if (FAILED(hr))
			{
				MessageBox::Show("Couldn't create sample grabber filter base!");
				return E_NOINTERFACE;
			} 
			

			cli::pin_ptr<ISampleGrabber *> ppGrabber = &pGrabber;
			
			pGrabberBaseFilter->QueryInterface(IID_ISampleGrabber, (void**)ppGrabber);
			
			AM_MEDIA_TYPE mt;
			ZeroMemory(&mt, sizeof(AM_MEDIA_TYPE));
			mt.majortype = MEDIATYPE_Video;
			mt.subtype = MEDIASUBTYPE_RGB24;
			hr = pGrabber->SetMediaType(&mt);
			FreeMediaType(mt);
			pGraphManager->AddFilter(pGrabberBaseFilter, L"eID Sample Grabber");

			cli::pin_ptr<ICaptureGraphBuilder2 *> ppCapture = &pCapture;
			hr = CoCreateInstance (
				 CLSID_CaptureGraphBuilder2, 
				 NULL, 
				 CLSCTX_INPROC_SERVER,
				 IID_ICaptureGraphBuilder2, 
				 (void **) ppCapture
				 );
			 if (FAILED(hr)){
				 MessageBox::Show("Initial ICaptureGraphBuilder2 failed!");
				 return E_NOINTERFACE;
			 }		

			 pCapture->SetFiltergraph(pGraphManager);
			 pCapture->RenderStream(
	//			&PIN_CATEGORY_CAPTURE,
				&PIN_CATEGORY_PREVIEW,
				 &MEDIATYPE_Video,
				 pCaptureBaseFilter,
				 pGrabberBaseFilter,
				 NULL
			 );

			 cli::pin_ptr<IVideoWindow *> ppVW = &pVW;
				cli::pin_ptr<IMediaControl *> ppMC = &pMC;
				cli::pin_ptr<IMediaEventEx *> ppME = &pME;

				hr = pGraphManager->QueryInterface(IID_IMediaControl,(LPVOID *) ppMC);
				 if (FAILED(hr)){
					MessageBox::Show("Initial IMediaControl failed!");
					return E_NOINTERFACE;
				 }
				 	 
				 hr = pGraphManager->QueryInterface(IID_IMediaEvent, (LPVOID *) ppME);
				 if (FAILED(hr)){
					MessageBox::Show("Initial IMediaEventEx failed!");
					return E_NOINTERFACE;
				 }
				 
				 hr = pGraphManager->QueryInterface(IID_IVideoWindow, (LPVOID *) ppVW);
				 if (FAILED(hr)){
					MessageBox::Show("Initial IVideoWindow failed!");
					return E_NOINTERFACE;
				 }	
			 return S_OK;
			
			
		}




	



