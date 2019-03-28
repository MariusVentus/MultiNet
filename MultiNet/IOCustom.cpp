#include "IOCustom.h"
#include <iostream>


bool IOCustom::YnQ(const std::string& out) const
{
	std::string answer;
	int errorCount = 0;
	while (errorCount < 3) {

		std::cout << out << "\n";
		std::cin.clear();
		std::getline(std::cin, answer);
		std::cout << "\n";
		if (answer == "y" || answer == "Y" || answer == "yes" || answer == "Yes") {
			return true;
		}
		else if (answer == "n" || answer == "N" || answer == "No" || answer == "no") {
			return false;
		}
		else {
			errorCount++;
			if (errorCount < 3) {
				std::cout << "I'm sorry, I didn't understand. Allow me to reset the question. Please Re-enter. \n\n";
			}
		}
	}
	std::cout << "I'm sorry, I didn't understand. Assuming No.\n \n";
	return false;
}

int IOCustom::HowMany(const std::string & out) const
{
	std::string answer;

	std::cout << "How many " << out << "?\n";
	std::getline(std::cin, answer);
	std::cout << "\n";
	return std::stoi(answer);
}

int IOCustom::HowMany(const std::string & out, int minR, int maxR) const
{
	std::string answer;
	int answerInt;
	bool isGood = false;
	
	while (!isGood)
	{
		std::cout << "How many " << out << " between " << minR << " and " << maxR << "?\n";
		std::getline(std::cin, answer);
		answerInt = std::stoi(answer);
		if (answerInt < minR || answerInt > maxR) {
			std::cout << "This does not appear to be in range. Please try again.\n";
		}
		else {
			isGood = true;
		}
	}
	std::cout << "\n";
	return answerInt;
}

void IOCustom::CleanLine(std::string& dataString)
{
	//Remove White Space
	while (dataString.find(" ") != std::string::npos) {
		dataString.erase(dataString.find(" "), 1);
	}
	//Remove Tabs
	while (dataString.find("\t") != std::string::npos) {
		dataString.erase(dataString.find("\t"), 1);
	}
	//Remove Double Commas
	while (dataString.find(",,") != std::string::npos) {
		dataString.erase(dataString.find(",,"), 1);
	}
	//Remove Leading Comma
	if (dataString.find(",") == 0) {
		dataString.erase(dataString.find(","), 1);
	}
	//Remove Trailing Comma
	if (dataString.find_last_of(",") == dataString.size() - 1 && !dataString.empty()) {
		dataString.pop_back();
	}

}
