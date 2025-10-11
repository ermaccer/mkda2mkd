// mkda2mkd.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include <iostream>
#include <fstream>
#include <memory>
#include <filesystem>
#include <vector>

struct mka_header_mkda {
    unsigned char header[4];
    char name[20] = {};
    int frames;
    int numData;
    int translationVector[3];
};

struct mka_header_mka {
    unsigned char header[4];
    char name[20] = {};
    int frames;
    int numData;
    int id;
    char pad[12] = {};
    int translationVector[3];
};


struct mka_entry {
    int type;
    int dataInfo;
    int offset;
};

int main(int argc, char* argv[])
{
    std::string input = argv[argc - 1];

    std::ifstream pFile(input, std::ifstream::binary);

    if (!pFile)
    {
        std::cout << "ERROR: Could not open " << input << "!" << std::endl;
        return 1;
    }

    
    mka_header_mkda header;
    pFile.read((char*)&header, sizeof(mka_header_mkda));

    if (!(header.header[3] == 0x31))
    {
        std::cout << "ERROR: " << input << "is not a MKDA Animation!" << std::endl;
        return 1;
    }

    std::cout << "Animation name: " << header.name << std::endl;



    std::vector<mka_entry> animInfo;

    for (int i = 0; i < header.numData + 1; i++)
    {
        mka_entry info;
        pFile.read((char*)&info, sizeof(mka_entry));
        animInfo.push_back(info);
    }


    mka_header_mka newHeader = {};
    newHeader.header[0] = header.header[0];
    newHeader.header[1] = header.header[1];
    newHeader.header[2] = header.header[2];
    newHeader.header[3] = 0x03;
    newHeader.id = -1;
    newHeader.frames = header.frames;
    sprintf_s(newHeader.name, header.name);
    newHeader.numData = header.numData;
    newHeader.translationVector[0] = header.translationVector[0];
    newHeader.translationVector[1] = header.translationVector[1];
    newHeader.translationVector[2] = header.translationVector[2];


    for (auto& i : animInfo)
       i.offset += sizeof(int) * 4;

    int dataSize = std::filesystem::file_size(input);
    std::unique_ptr<char[]> dataBuff = std::make_unique<char[]>(dataSize);
    pFile.read(dataBuff.get(), dataSize);

    pFile.close();

    std::string output = header.name; 

    std::ofstream oFile(output, std::ofstream::binary);

    oFile.write((char*)&newHeader, sizeof(mka_header_mka));
    for (auto& i : animInfo)
        oFile.write((char*)&i, sizeof(mka_entry));

    oFile.write(dataBuff.get(), dataSize);

    std::cout << "Finished." << std::endl;

    return 0;
}

// Run program: Ctrl + F5 or Debug > Start Without Debugging menu
// Debug program: F5 or Debug > Start Debugging menu

// Tips for Getting Started: 
//   1. Use the Solution Explorer window to add/manage files
//   2. Use the Team Explorer window to connect to source control
//   3. Use the Output window to see build output and other messages
//   4. Use the Error List window to view errors
//   5. Go to Project > Add New Item to create new code files, or Project > Add Existing Item to add existing code files to the project
//   6. In the future, to open this project again, go to File > Open > Project and select the .sln file
