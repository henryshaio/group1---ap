#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <curl/curl.h>
#include <errno.h>
#include <unistd.h>
#include "update.h"
#include "utility.h"
void UpdateAllPackage()
{
	system("opkg update; opkg upgrade;");
}
size_t Write_Callback(void *buffer, size_t size, size_t nmemb, void *stream)
{
	struct FTPFile *out = (struct FTPFile *)stream;
	if(out && !out->stream)
	{
		/* open file for writing */ 
		out->stream = fopen(out->filename, "wb");
		if(!out->stream)
			return -1; /* failure, can't open file to write */ 
	}
	return fwrite(buffer, size, nmemb, out->stream);
}
size_t Read_Callback(void * ptr, size_t size, size_t nmemb, void * stream)
{
	curl_off_t nread;
	size_t retcode = fread(ptr, size, nmemb, stream);
	nread = (curl_off_t)retcode;

	return retcode;
}
void DownloadFile(const char * URL, const char * username, const char * password, const char * DownloadLoc)
{
	char logText[500];

	CURL *curl;
	CURLcode res;
	struct FTPFile ftpfile =
	{
		"backup.tar.gz",
		NULL
	};
	ftpfile.filename = DownloadLoc;

	curl_global_init(CURL_GLOBAL_DEFAULT);
	curl = curl_easy_init();

	if(curl)
	{
		/*
		* You better replace the URL with one that works!
		*/ 
		curl_easy_setopt(curl, CURLOPT_URL, URL);//"ftp://speedtest.tele2.net"
		/* Define our callback to get called when there's data to be written */ 
		curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, Write_Callback);
		/* Set a pointer to our struct to pass to the callback */ 
		curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ftpfile);

		char buf[1000] = {};
		snprintf(buf, sizeof(buf), "%s:%s", username, password);
		curl_easy_setopt(curl, CURLOPT_USERPWD, buf);


		/* Switch on full protocol/debug output */ 
		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		res = curl_easy_perform(curl);

		/* always cleanup */ 
		curl_easy_cleanup(curl);

		if(CURLE_OK != res)
		{
			/* we failed */ 
			snprintf(logText, sizeof(logText), "[update.c] Curl error message: %s", curl_easy_strerror(res));
			RecordLog(logText);
		}
	}
	 
	if(ftpfile.stream)
		fclose(ftpfile.stream); /* close the local file */ 
	 
	curl_global_cleanup();

}
void UploadFile(const char * UploadURL, const char * username, const char * password, const char * FileLoc)
{
	char logText[500];
	CURL * curl;
	CURLcode res;
	FILE * hd_src;
	struct stat file_info;
	curl_off_t fsize;
	
	if(stat(FileLoc, &file_info))
	{
		snprintf(logText, sizeof(logText), "[update.c] Couldn't open %s", FileLoc);
		RecordLog(logText);
		return;
	}

	fsize = (curl_off_t)file_info.st_size;

	hd_src = fopen(FileLoc, "rb");

	curl_global_init(CURL_GLOBAL_ALL);

	curl = curl_easy_init();
	if(curl)
	{
		/* we want to use our own read function */ 
		curl_easy_setopt(curl, CURLOPT_READFUNCTION, Read_Callback);

		/* enable uploading */ 
		curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);

		/* specify target */ 
		curl_easy_setopt(curl, CURLOPT_URL, UploadURL);//"ftp://speedtest.tele2.net/upload"

		/* now specify which file to upload */ 
		curl_easy_setopt(curl, CURLOPT_READDATA, hd_src);

		char buf[1000] = {};
		snprintf(buf, sizeof(buf), "%s:%s", username, password);
		curl_easy_setopt(curl, CURLOPT_USERPWD, buf);


		/* Set the size of the file to upload (optional).  If you give a *_LARGE
		option you MUST make sure that the type of the passed-in argument is a
		curl_off_t. If you use CURLOPT_INFILESIZE (without _LARGE) you must
		make sure that to pass in a type 'long' argument. */ 
		curl_easy_setopt(curl, CURLOPT_INFILESIZE_LARGE,
			     (curl_off_t)fsize);

		curl_easy_setopt(curl, CURLOPT_VERBOSE, 1L);

		/* Now run off and do what you've been told! */ 
		res = curl_easy_perform(curl);
		/* Check for errors */ 
		if(res != CURLE_OK)
		{
			snprintf(logText, sizeof(logText), "[update.c] Curl error message: %s", curl_easy_strerror(res));
			RecordLog(logText);
		}

		/* always cleanup */ 
		curl_easy_cleanup(curl);
	}
	fclose(hd_src); /* close the local file */ 

	curl_global_cleanup();

}
