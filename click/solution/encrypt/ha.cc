#include <bits/stdc++.h>
#include <fstream>
#include <string>
#include <stdio.h>
#include <openssl/md5.h>

using namespace std;

const string key = "[]A/A]A^A_";

string check()
{
    unsigned char c[MD5_DIGEST_LENGTH];
    const string filename = "../userlevel/click";
    int i;
    FILE *inFile = fopen (filename.c_str(), "rb");
    MD5_CTX mdContext;
    int bytes;
    unsigned char data[1024];

    if (inFile == NULL) {
        printf ("%s can't be opened.\n", filename.c_str());
        return "-";
    }

    MD5_Init (&mdContext);
    while ((bytes = fread (data, 1, 1024, inFile)) != 0)
        MD5_Update (&mdContext, data, bytes);
    MD5_Final (c,&mdContext);
    std::ostringstream s;
    s << std::hex << std::setfill('0') << std::nouppercase;
    for(i = 0; i < MD5_DIGEST_LENGTH; i++) s << std::setw(2) << static_cast<int>(c[i]);
//    printf (" %s\n", filename);
//    fclose (inFile);
    return s.str();
}

string make_config(string msg, string key) {
	string tmp(key);

	while (key.size() < msg.size()) {
		key += tmp;
	}

	for (string::size_type i = 0; i < msg.size(); ++i) {
		msg[i] ^= key[i];
	}

	return msg;
}

int main (int argc, char* argv[]) {
	if (check() != "ff841a17e523bc4501de534c2ef7171e") {
		cerr << "Replaced reference click binary with something else. Will not start." << endl;
		return -1;
	}
	streampos size;
	char * memblock;
	ifstream file1 ("ha.enc", ios::in|ios::binary|ios::ate);

	if (file1.is_open()) {
		size = file1.tellg();
		memblock = new char [size];
		file1.seekg (0, ios::beg);
		file1.read (memblock, size);
		file1.close();

		string enc(memblock, size);
		delete[] memblock;
			
		string config = make_config(enc, key);
		const std::string command = "echo '" + config + "' | ../userlevel/click -p 10002";
		system(command.c_str());
	} else {
		cout << "Unable to open file" << endl;
	}
	
	return 0;
}
