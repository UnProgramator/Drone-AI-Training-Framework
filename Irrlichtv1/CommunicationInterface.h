#pragma once

#include <string>
#include <vector>
#include <typeinfo>

class DataCoolectorInterface {
public:
	virtual void init_parser(int elementCount) = 0;
	virtual void parse_double(const char* name, const double value) = 0;
	virtual void parse_double_array(const char* name, const std::vector<double>& value) = 0;
};

//interface to be implemented and given back to the library in order to comunicate with the ai script
template<typename ReturnedValueFromStript, typename FeedbackType>
class CommunicationInterface {
public:
	virtual ReturnedValueFromStript call() =0;
	virtual void give_feedback(const FeedbackType & newParams) = 0;
};

struct default_FeedbackType {
	double new_pos[3];
	double destination[3];
	bool hasCollide;
	default_FeedbackType() = default;
};

struct default_ReturnedValueFromStript {
	float forward;
	float rotation_angle;
	float up;
};

typedef CommunicationInterface< default_ReturnedValueFromStript, default_FeedbackType> default_CommunicationInterface;