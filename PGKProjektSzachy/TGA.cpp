#include "TGA.hpp"
#define _CRT_SECURE_NO_WARNINGS

Tga Tga::loadFileTGA(const char *filename)
{
    Tga tgaFile;    
    FILE *filePtr;

    unsigned char ucharBad;
    short int sintBad;
    long imageSize;
    int colorMode;
    unsigned char colorSwap;
    
    fopen_s(&filePtr, filename, "rb");

    if (filePtr == NULL)
    {
        std::cout << "Nie mozna otworzyc pliku TGA:" << filename << std::endl;
        exit(0);
    }
        
    // Dwa pierwsze bajty które nie s¹ potrzebne
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
        
    // Jaki typ obrazu jest przechowany w imageTypeCode
    fread(&tgaFile.imageTypeCode, sizeof(unsigned char), 1, filePtr);
        
    // Obs³ugiwany typ obrazu
    if (tgaFile.imageTypeCode != 2)
    {
        std::cout << "Z³y format TGA:" << filename << std::endl;
        fclose(filePtr);

        exit(0);
    }
        
    // Kolejne niepotrzebne bajty
    fread(&sintBad, sizeof(short int), 1, filePtr);
    fread(&sintBad, sizeof(short int), 1, filePtr);
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
    fread(&sintBad, sizeof(short int), 1, filePtr);
    fread(&sintBad, sizeof(short int), 1, filePtr);
        
    // Rozmiar obrazu
    fread(&tgaFile.imageWidth, sizeof(short int), 1, filePtr);
    fread(&tgaFile.imageHeight, sizeof(short int), 1, filePtr);
        
    // G³êbia
    fread(&tgaFile.bitCount, sizeof(unsigned char), 1, filePtr);
        
    // Kolejne niepotrzebne bajty
    fread(&ucharBad, sizeof(unsigned char), 1, filePtr);
        
    // Tryb koloru
    colorMode = tgaFile.bitCount / 8;

    imageSize = tgaFile.imageWidth * tgaFile.imageHeight * colorMode;
        
    // Alokowanie pamiêci dla obrazu
    tgaFile.imageData = (unsigned char*)malloc(sizeof(unsigned char)*imageSize);
        
    // Odczytywanie danych obrazu
    fread(tgaFile.imageData, sizeof(unsigned char), imageSize, filePtr);
        
    // Zmiana trybu koloru z BGR do RGB.
    // OpenGL rozumie RGB
    for (int imageIdx = 0; imageIdx < imageSize; imageIdx += colorMode)
    {
        colorSwap = tgaFile.imageData[imageIdx];
        tgaFile.imageData[imageIdx] = tgaFile.imageData[imageIdx + 2];
        tgaFile.imageData[imageIdx + 2] = colorSwap;
    }
        
    fclose(filePtr);

    return tgaFile;
}