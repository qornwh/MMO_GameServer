#pragma once
#include "pch.h"

class ApiUtils
{
	// header �� "Content-Type", "application/json" ����
public:
	static cpr::Response Post(const std::string& url, const std::string& body)
	{
		cpr::Response response = cpr::Post(
			cpr::Url{ url },
			cpr::Body{ body },
			cpr::Header{ { "Content-Type", "application/json" } });
		return response;
	}
};

