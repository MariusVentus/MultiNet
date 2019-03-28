#pragma once
#include <string>

class IOCustom {
public:
	bool YnQ(const std::string& out) const;
	int HowMany(const std::string& out) const;
	int HowMany(const std::string& out, int minR, int maxR) const;
	void CleanLine(std::string& dataString);

};