// MyJsonIterator.cpp : This file contains the 'main' function. Program execution begins and ends there.
//

#include "pch.h"
#include <iostream>
#include <unordered_map>
#include <string>

class JsonParser {
	// ключ, значение, следующий ключ
	std::string json;
	std::unordered_map<std::string, std::pair<std::string, std::string>> m;

	JsonParser() {
		std::string my_json;
		int moment = 0;
		for (auto& el : my_json) {
			if (moment == 0) {
				if (el == ' ') {
					continue;
				}
				else if (el == '{') {
					moment = 1;
				}
				else {
					// bad parce
				}
			}
			if (moment == 1) {
				if (el == ' ') {
					continue;
				}
				else if (el == '\"') {
					moment = 2;
				}
				else {
					// bad parce
				}
			}
			if (moment == 2) {
				if (el == '\"') {
					moment = 4;
				}
				my_json += el;
			}
			if (moment == 4) {
				if (el == ' ') {
					continue;
				}
				else if (el == '}') {
					// good
					return;
				}
				else if (el == ':') {
					// add 
					moment = 5;
				}
				else if (el == ',') {
					// add
					moment = 5;
				}
				// bad parce
			}
			if (moment == 5) {
				if (el == ' ') {
					continue;
				}
				else if (el == '\"') {
					moment = 2;
				}
				// bad parce
			}
		}
	}

	bool is_valide() {
		for (int i = 0; i < json.size(); ++i) {
			if ()
		}
	}
};

int main()
{
    
}
