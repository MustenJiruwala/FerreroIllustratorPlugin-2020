#include "IllustratorSDK.h"
#include "FerreroServerFacade.h"
#include "curl/curl.h"
#include <stdio.h>
#include "MenuPlayID.h"
#include "ArtworkUtil.h"
#include "Logger.h"
#include <sys/stat.h>
#include <fcntl.h>
FerreroServerFacade::FerreroServerFacade()
{

}

// Structure to hold response buffer data
struct responseString
{
	char *ptr;
	size_t nLength;
};

size_t writeStringResponse(void *ptr, size_t nSize, size_t nMemBytes, struct responseString *psResponse)
{
	string strBufferSize;
	strBufferSize.append(std::to_string(static_cast<int>(nSize * nMemBytes)));
	size_t nBytesTakenCareOf = 0;
	do
	{
		// resize the buffer based on response length
		size_t nNewLen = psResponse->nLength + nSize * nMemBytes;
		char *pcNewBuffer = (char*)realloc(psResponse->ptr, nNewLen + 1);
		if (pcNewBuffer == NULL)
			break;
		psResponse->ptr = pcNewBuffer;
		// copy the response
		memcpy(psResponse->ptr + psResponse->nLength, ptr, nSize * nMemBytes);
		psResponse->ptr[nNewLen] = '\0';
		psResponse->nLength = nNewLen;
		nBytesTakenCareOf = nSize * nMemBytes;
	} while (false);
	return nBytesTakenCareOf;
}

// Initializes the structure to hold response data 
void init_string(struct responseString *psResponse)
{
	do
	{
		psResponse->nLength = 0;
		psResponse->ptr = (char*)malloc(psResponse->nLength + 1);
		if (psResponse->ptr == NULL)
			break;
		psResponse->ptr[0] = '\0';
	} while (false);
}

std::string FerreroServerFacade::CallServlet(string strBaseURL, string strServletPath, string strParams, string &strErrorString)
{
	std::string strRespMsg = "";
	// server call by get method
	std::string strURL = strBaseURL;

	// prepare the URL
	strURL.append(strServletPath);
	strURL.append("?");
	strURL.append(strParams);
    Logger::Info("FerreroServerFacade", "CallServlet : strURL :", strURL);
	CURL *poCurl;
	CURLcode tCurlStatus;
	ArtworkUtil oUtil;
	poCurl = curl_easy_init();
	if (poCurl)
	{
		/* initialize curl
		*/
		struct curl_slist *poHeaders = NULL;
		struct responseString psResponse;
		init_string(&psResponse);
		poHeaders = curl_slist_append(poHeaders, "Accept: application/xml");
		poHeaders = curl_slist_append(poHeaders, "Accept-Charset: UTF-8");
		curl_easy_setopt(poCurl, CURLOPT_URL, strURL.c_str());
		curl_easy_setopt(poCurl, CURLOPT_SSL_VERIFYHOST, FALSE);
		curl_easy_setopt(poCurl, CURLOPT_SSL_VERIFYPEER, FALSE);
		curl_easy_setopt(poCurl, CURLOPT_HTTPGET, 1);
		curl_easy_setopt(poCurl, CURLOPT_WRITEDATA, &psResponse);
		curl_easy_setopt(poCurl, CURLOPT_WRITEFUNCTION, writeStringResponse);
		curl_easy_setopt(poCurl, CURLOPT_HTTPHEADER, poHeaders);
		curl_easy_setopt(poCurl, CURLOPT_COOKIEFILE, "");
		curl_easy_setopt(poCurl, CURLOPT_CONNECTTIMEOUT, 10L);
		long http_code = 0;
		char *errbuf = new char[CURL_ERROR_SIZE];
		curl_easy_setopt(poCurl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(poCurl, CURLOPT_TCP_KEEPALIVE, 1L);
		string strProxy = GetProxyString();
		if (strProxy.length() > 0)
		{
			std::vector<string>arrComponent = oUtil.Split(strProxy, ":");
			curl_easy_setopt(poCurl, CURLOPT_PROXY, arrComponent.size() > 0 ? arrComponent.at(0).c_str() : "");
			curl_easy_setopt(poCurl, CURLOPT_PROXYPORT, arrComponent.size() > 1 ? arrComponent.at(1).c_str() : "");
			curl_easy_setopt(poCurl, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
			/* set the proxy credentials */
			string strUserPassword = BuildUserPass(arrComponent);
			curl_easy_setopt(poCurl, CURLOPT_PROXYUSERPWD, strUserPassword.length() > 0 ? strUserPassword.c_str() : "");
		}
		/* provide a buffer to store errors in */
		curl_easy_setopt(poCurl, CURLOPT_ERRORBUFFER, errbuf);
		tCurlStatus = curl_easy_perform(poCurl);
        Logger::Info("FerreroServerFacade", "CallServlet : tCurlStatus :", to_string(tCurlStatus));
		curl_easy_getinfo(poCurl, CURLINFO_RESPONSE_CODE, &http_code);
        Logger::Info("FerreroServerFacade", "CallServlet : http_code :", to_string(http_code));
		std::string strResp(psResponse.ptr, psResponse.nLength);
		strRespMsg = strResp;
		if (http_code != 200)
		{
			strErrorString = "Server call failed with http_code :" + to_string(http_code) + "\n";
			strErrorString.append("Url :");
			strErrorString.append(strURL);
			strErrorString.append("\n");
			strErrorString.append("Response :");
			strErrorString.append(strRespMsg);
			strErrorString.append("\n");
		}
		delete[] errbuf;
		// Cleanup
		free(psResponse.ptr);
		curl_easy_cleanup(poCurl);
	}
	return strRespMsg;
}

size_t write_data(void *ptr, size_t size, size_t nmemb, FILE *stream)
{
	size_t written = fwrite(ptr, size, nmemb, stream);
	return written;
}

void FerreroServerFacade::download(string strBaseURL, string strServletPath, string strParams, string strProjectId, string &strErrorString, bool bIsForArtworkCreation)
{
	CURL *curl;
	FILE *fp;
	CURLcode res;
	std::string strURL = strBaseURL;
	std::string strParam = strParams;
	// prepare the URL
	strURL.append(strServletPath);
	strURL.append("?");
	strURL.append(strParams);
    Logger::Info("FerreroServerFacade", "download : URL :", strURL);
	ArtworkUtil oUtil;
	std::string strOutFileName = oUtil.GetUserFolder ();
	if (bIsForArtworkCreation)
	{
		strOutFileName.append(kArtworkFolder);
		oUtil.CreateFolder(strOutFileName);
		strOutFileName.append(1, kFilePathSeparator);
	}
	strOutFileName.append(strProjectId);
	strOutFileName.append(".zip");
    Logger::Info("FerreroServerFacade", "download : strOutFileName :", strOutFileName);
	curl = curl_easy_init();
	if (curl) 
	{
#ifdef _WINDOWS
		fopen(&fp, strOutFileName.c_str(), "wb");
#else
        fp = fopen (strOutFileName.c_str(), "wb");
#endif
		curl_easy_setopt(curl, CURLOPT_URL, strURL.c_str());
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_data);
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, FALSE);
        curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);
		curl_easy_setopt(curl, CURLOPT_TCP_KEEPALIVE, 1);
		string strProxy = GetProxyString();
		if (strProxy.length() > 0)
		{
			std::vector<string>arrComponent = oUtil.Split(strProxy, ":");
			curl_easy_setopt(curl, CURLOPT_PROXY, arrComponent.size() > 0 ? arrComponent.at(0).c_str() : "");
			curl_easy_setopt(curl, CURLOPT_PROXYPORT, arrComponent.size() > 1 ? arrComponent.at(1).c_str() : "");
			curl_easy_setopt(curl, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
			/* set the proxy credentials */
			string strUserPassword = BuildUserPass(arrComponent);
			curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, strUserPassword.length() > 0 ? strUserPassword.c_str() : "");
		}
		res = curl_easy_perform(curl);
        Logger::Info("FerreroServerFacade", "download : CURLcode :", to_string(res));

        long http_code = 0;
        curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
        Logger::Info("FerreroServerFacade", "download : http_code :", to_string(http_code));
		if (http_code != 200)
		{
			strErrorString = "download failed with http_code :" + to_string(http_code) + " and CURLCode :"+ to_string(res) + "\n";
			strErrorString.append("Url :");
			strErrorString.append(strURL);
			strErrorString.append("\n");
		}
		/* always cleanup */
		curl_easy_cleanup(curl);
		if(fp != nullptr)
			fclose(fp);
	}
}

size_t read_data(char *bufptr, size_t size, size_t nitems, void *userp)
{
	size_t read;
	read = fread(bufptr, size, nitems, (FILE*)userp);
	return read;
}

int FerreroServerFacade::UploadFile(string strFilePath, string strTemplateID, string &strErrorString, bool bIsForArtwork)
{
    Logger::Info("FerreroServerFacade", "UploadFile : strFilePath :", strFilePath);
    Logger::Info("FerreroServerFacade", "UploadFile : strTemplateID :", strTemplateID);
	int nError = 1;
	string strFileName = strTemplateID;
	strFileName.append(".zip");

	CURL *curl;
	CURLM *multi_handle;
	int still_running = 0;
	struct curl_httppost *formpost = NULL;
	struct curl_httppost *lastptr = NULL;
	struct curl_slist *headerlist = NULL;
	ArtworkUtil oUtil;
	static const char buf[] = "Expect:";
	do
	{
		/* Fill in the file upload field. This makes libcurl load data from
		   the given file name when curl_easy_perform() is called. */
		curl_formadd(&formpost,
				&lastptr,
				CURLFORM_COPYNAME, "sendfile",
				CURLFORM_FILE, strFilePath.c_str(),
				CURLFORM_END);
		/* Fill in the submit field too, even if this is rarely needed */
		curl_formadd(&formpost,
			&lastptr,
			CURLFORM_COPYNAME, "submit",
			CURLFORM_COPYCONTENTS, "send",
			CURLFORM_END);

		curl = curl_easy_init();
		multi_handle = curl_multi_init();
		headerlist = curl_slist_append(headerlist, buf);
		string strURL = kFerreroWFESS2URL;
		strURL.append("/SapRequestServlet?");
		if(bIsForArtwork)
			strURL.append("function=UploadArtwork&artworkid=");
		else
			strURL.append("function=upload&templateid=");
		strURL.append(strTemplateID);
        Logger::Info("FerreroServerFacade", "UploadFile : strURL :", strURL);
		if (curl && multi_handle) 
		{
			curl_easy_setopt(curl, CURLOPT_URL, strURL.c_str());
			curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);
			curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headerlist);
			curl_easy_setopt(curl, CURLOPT_HTTPPOST, formpost);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYHOST, FALSE);
            curl_easy_setopt(curl, CURLOPT_SSL_VERIFYPEER, FALSE);
			string strProxy = GetProxyString();
			if (strProxy.length() > 0)
			{
				std::vector<string>arrComponent = oUtil.Split(strProxy, ":");
				curl_easy_setopt(curl, CURLOPT_PROXY, arrComponent.size () > 0 ? arrComponent.at(0).c_str() : "");
				curl_easy_setopt(curl, CURLOPT_PROXYPORT, arrComponent.size() > 1 ? arrComponent.at(1).c_str() : "");
				curl_easy_setopt(curl, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
				/* set the proxy credentials */
				string strUserPassword = BuildUserPass(arrComponent);
				curl_easy_setopt(curl, CURLOPT_PROXYUSERPWD, strUserPassword.length() > 0 ? strUserPassword.c_str() : "");
			}
			curl_multi_add_handle(multi_handle, curl);
			curl_multi_perform(multi_handle, &still_running);
			while (still_running) 
			{
				struct timeval timeout;
				int rc;
				CURLMcode mc;
				fd_set fdread;
				fd_set fdwrite;
				fd_set fdexcep;
				int maxfd = -1;
				long curl_timeo = -1;
				FD_ZERO(&fdread);
				FD_ZERO(&fdwrite);
				FD_ZERO(&fdexcep);
				timeout.tv_sec = 1;
				timeout.tv_usec = 0;
				curl_multi_timeout(multi_handle, &curl_timeo);
				if (curl_timeo >= 0) {
					timeout.tv_sec = curl_timeo / 1000;
					if (timeout.tv_sec > 1)
						timeout.tv_sec = 1;
					else
						timeout.tv_usec = (curl_timeo % 1000) * 1000;
				}
				mc = curl_multi_fdset(multi_handle, &fdread, &fdwrite, &fdexcep, &maxfd);
				if (mc != CURLM_OK) {
					fprintf(stderr, "curl_multi_fdset() failed, code %d.\n", mc);
					break;
				}
				if (maxfd == -1) {
#ifdef _WIN32
					Sleep(100);
					rc = 0;
#else
					struct timeval wait = { 0, 100 * 1000 }; 
					rc = select(0, NULL, NULL, NULL, &wait);
#endif
				}
				else 
				{
					rc = select(maxfd + 1, &fdread, &fdwrite, &fdexcep, &timeout);
				}
				switch (rc) 
				{
				case -1:
					break;
				case 0:
				default:
					printf("perform!\n");
					curl_multi_perform(multi_handle, &still_running);
					printf("running: %d!\n", still_running);
					break;
				}
			}
			curl_multi_cleanup(multi_handle);
			curl_easy_cleanup(curl);
			curl_formfree(formpost);
			curl_slist_free_all(headerlist);
            nError = 0;
		}
	} while (false);
    Logger::Info("FerreroServerFacade", "UploadFile : nError :", to_string(nError));
	return nError;
}

std::string FerreroServerFacade::URLEncode(string strParams)
{
	std::string strRespMsg = strParams;
	Logger::Info("FerreroServerFacade", "URLEncode : strParams :", strParams);
	do
	{
		if (strRespMsg.length() == 0)
			break;
		CURL *poCurl;
		poCurl = curl_easy_init();
		if (poCurl)
		{
			strRespMsg = "";
			char *output = curl_easy_escape(poCurl, strParams.c_str(), strParams.length());
			strRespMsg.append(output);
			if (output)
			{
				Logger::Info("FerreroServerFacade", "URLEncode : output :", output);
				curl_free(output);
			}
			curl_easy_cleanup(poCurl);
		}
	} while (false);
	return strRespMsg;
}

string FerreroServerFacade::GetProxyString()
{
	Logger::Info("FerreroServerFacade", "GetProxyString", "");
	ArtworkUtil oUtil;
	string strConFigFilePath = oUtil.GetUserFolder();
	strConFigFilePath.append(kProxyConfigFileName);
	string strProxy = oUtil.GetFileContent(strConFigFilePath);
	Logger::Info("FerreroServerFacade", "GetProxyString : strProxy = ", strProxy);
	return strProxy;
}

string FerreroServerFacade::BuildUserPass(std::vector<string>arrComponent)
{
	string strUserPassword = "";
	if (arrComponent.size() > 3)
	{
		strUserPassword.append(arrComponent.at(2));
		strUserPassword.append(":");
		strUserPassword.append(arrComponent.at(3));
	}
	Logger::Info("FerreroServerFacade", "BuildUserPass : strUserPassword = ", strUserPassword);
	return strUserPassword;
}

std::string FerreroServerFacade::CallServletUsingPostRequest(string strBaseURL, string strServletPath, string strParams, string &strErrorString)
{
	std::string strRespMsg = "";
	// server call by get method
	std::string strURL = strBaseURL;

	// prepare the URL
//	strURL.append(strServletPath);
//	strURL.append("?");
//	strURL.append(strParams);
	Logger::Info("FerreroServerFacade", "CallServlet : strURL :", strURL);
	CURL *poCurl;
	CURLcode tCurlStatus;
	ArtworkUtil oUtil;
	poCurl = curl_easy_init();
	if (poCurl)
	{
		/* initialize curl
		*/
		struct curl_slist *poHeaders = NULL;
		struct responseString psResponse;
		init_string(&psResponse);
		poHeaders = curl_slist_append(poHeaders, "Accept: application/xml");
		poHeaders = curl_slist_append(poHeaders, "Accept-Charset: UTF-8");
		curl_easy_setopt(poCurl, CURLOPT_URL, strURL.c_str());
		curl_easy_setopt(poCurl, CURLOPT_POSTFIELDS, strParams.c_str());
		curl_easy_setopt(poCurl, CURLOPT_SSL_VERIFYHOST, FALSE);
		curl_easy_setopt(poCurl, CURLOPT_SSL_VERIFYPEER, FALSE);
		curl_easy_setopt(poCurl, CURLOPT_HTTPPOST, 1);
		curl_easy_setopt(poCurl, CURLOPT_WRITEDATA, &psResponse);
		curl_easy_setopt(poCurl, CURLOPT_WRITEFUNCTION, writeStringResponse);
		curl_easy_setopt(poCurl, CURLOPT_HTTPHEADER, poHeaders);
		curl_easy_setopt(poCurl, CURLOPT_COOKIEFILE, "");
		curl_easy_setopt(poCurl, CURLOPT_CONNECTTIMEOUT, 10L);
		long http_code = 0;
		char *errbuf = new char[CURL_ERROR_SIZE];
		curl_easy_setopt(poCurl, CURLOPT_FOLLOWLOCATION, 1);
		curl_easy_setopt(poCurl, CURLOPT_TCP_KEEPALIVE, 1L);
		string strProxy = GetProxyString();
		if (strProxy.length() > 0)
		{
			std::vector<string>arrComponent = oUtil.Split(strProxy, ":");
			curl_easy_setopt(poCurl, CURLOPT_PROXY, arrComponent.size() > 0 ? arrComponent.at(0).c_str() : "");
			curl_easy_setopt(poCurl, CURLOPT_PROXYPORT, arrComponent.size() > 1 ? arrComponent.at(1).c_str() : "");
			curl_easy_setopt(poCurl, CURLOPT_PROXYAUTH, CURLAUTH_ANY);
			/* set the proxy credentials */
			string strUserPassword = BuildUserPass(arrComponent);
			curl_easy_setopt(poCurl, CURLOPT_PROXYUSERPWD, strUserPassword.length() > 0 ? strUserPassword.c_str() : "");
		}
		/* provide a buffer to store errors in */
		curl_easy_setopt(poCurl, CURLOPT_ERRORBUFFER, errbuf);
		tCurlStatus = curl_easy_perform(poCurl);
		Logger::Info("FerreroServerFacade", "CallServlet : tCurlStatus :", to_string(tCurlStatus));
		curl_easy_getinfo(poCurl, CURLINFO_RESPONSE_CODE, &http_code);
		Logger::Info("FerreroServerFacade", "CallServlet : http_code :", to_string(http_code));
		std::string strResp(psResponse.ptr, psResponse.nLength);
		strRespMsg = strResp;
		if (http_code != 200)
		{
			strErrorString = "Server call failed with http_code :" + to_string(http_code) + "\n";
			strErrorString.append("Url :");
			strErrorString.append(strURL);
			strErrorString.append("\n");
			strErrorString.append("Response :");
			strErrorString.append(strRespMsg);
			strErrorString.append("\n");
		}
		delete[] errbuf;
		// Cleanup
		free(psResponse.ptr);
		curl_easy_cleanup(poCurl);
	}
	return strRespMsg;
}
