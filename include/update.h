#ifndef UPDATE_H
	#define UPDATE_H


	struct FTPFile {
	  const char *filename;
	  FILE *stream;
	};
	void UpdateAllPackage();
	void DownloadSettingPackage(const char * URL, const char * username, const char * password, const char * DownloadLoc);
	void UploadSettingPackage(const char * UploadURL, const char * username, const char * password, const char * FileLoc);
#endif
