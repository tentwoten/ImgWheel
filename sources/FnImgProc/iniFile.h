#ifndef INI_FILE_H_
#define INI_FILE_H_

#ifdef __cplusplus
extern "C"
{
#endif

	//int read_profile_string(const char* section, const char* key, char* value, int size, const char* default_value, const char* file);
	int ReadString_INI(const char* section, const char* key, char* value, int size, const char* file, const char* default_value = 0);
	int ReadInt_INI(const char* section, const char* key, const char* file, int default_value = 0);
	float ReadFloat_INI(const char* section, const char* key, const char* file, float default_value = 0.0f);
	double ReadDouble_INI(const char* section, const char* key, const char* file, double default_value = 0);


	int WriteInt_INI(const char* section, const char* key, int value, const char* file);
	int WriteFloat_INI(const char* section, const char* key, float value, const char* file);
	int WriteString_INI(const char* section, const char* key, const char* value, const char* file);

#ifdef __cplusplus
};
#endif
#endif //end of INI_FILE_H_
