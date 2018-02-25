#include "Arduino.h"
#include "Storage.h"
#include <EEPROM.h>

int size = 512;
byte dataLength = 18;

Storage::Storage(){}

int Storage::getMaxSlots(){
	return size / dataLength;
}

int Storage::getSize(){
	return size;
}

int getStartIndex(int slot){
	return slot <= (size/dataLength) ? dataLength * slot : -1;
}

void Storage::clearAll(){
	for (int i = 0; i < size; i++){
		EEPROM.write(i, 0);
	}
}

void clearSlot(int slot){
	int start = getStartIndex(slot);
	int end = start + dataLength;	
	for (int i = start; i < end; i++){
		EEPROM.write(i, 0);
	}
}

void Storage::saveStringData(int slot, String data){
	clearSlot(slot);	
	int start = getStartIndex(slot);
	int end = start + dataLength;
	int charIndex = 0;	
	for (int i = start; i < end; ++i){
		EEPROM.write(i, data[charIndex]);
		if( charIndex+1 >= data.length() ){
			break;
		}
		charIndex++;
	}
}

void Storage::saveData(int slot, const char *data){
	clearSlot(slot);
	int start = getStartIndex(slot);
	int end = start + dataLength;
	int charIndex = 0;
	size_t sz = strlen(data);
	for (int i = start; i < end; i++){
		EEPROM.write(i, data[charIndex]);
		charIndex++;
		if( charIndex >= sz ){
			break;
		}		
	}
}

void Storage::clearSlot(int slot){
	int start = getStartIndex(slot);
	int end = start + dataLength;	
	for (int i = start; i < end; i++){
		EEPROM.write(i, 0);
	}
}

bool Storage::isEmpty(int slot){
	int start = getStartIndex(slot);
	int val = EEPROM.read(start);
	return val <= 0 || val > 126;
}

void Storage::getData(int slot, char *buff){
	int start = getStartIndex(slot);
	int end = start + dataLength;	
	int charIndex = 0;
	for (int i = start; i < end; i++){
		int c = EEPROM.read(i);		
		if( c >= 32 && c <= 126 ){
			buff[charIndex] = char(c);
			charIndex++;
		}else{
			break;			
		}	
	}
	buff[charIndex] = '\0';
}

int Storage::getSlotSize(int slot){
	int start = getStartIndex(slot);
	int end = start + dataLength;	
	int charIndex = 0;
	for (int i = start; i < end; i++){
		int c = EEPROM.read(i);
		if( c >= 32 && c <= 126 ){
			charIndex++;
		}
	}
	return charIndex;
}