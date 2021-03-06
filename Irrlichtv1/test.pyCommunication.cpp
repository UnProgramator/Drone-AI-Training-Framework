#include "test.pyCommunication.h"
#include "CommunicationInterface.h"
#include <Python.h>
#include <string>
#include <stdio.h>
#include <iostream>

using std::operator""s;
#define CHECK_CREATED(var) if(!var) throw std::exception(("python variable "s+ #var +" at line  " + std::to_string(__LINE__) + " in file " + __FILE__ +" could not be created").c_str())

pyCommunication::pyCommunication(const wchar_t* folder, const char* path, uint16_t input_size, uint16_t output_size, float learning_rate):
	func_arg(nullptr), is_running(false), crt_element(0),
	call_func{ nullptr }, results_fuc{nullptr}
{
	if (!path)
		throw std::exception("Error: path to python file not given");
	Py_Initialize();

	module = PyImport_ImportModule(path);
	if (!module) {
		finishCommunication();
		throw std::exception(("Error: module "s + path + " could not be imported").c_str());
	}
	is_running = true;

	call_func = PyObject_GetAttrString(module, "next_step");
	if (!call_func) {
		finishCommunication();
		throw std::exception("Error: object/function iterateNextGeneration not Found");
	}
	if (!PyCallable_Check(call_func)) {
		finishCommunication();
		throw std::exception("Error: object/function iterateNextGeneration not callable");
	}

	results_fuc = PyObject_GetAttrString(module, "get_feedback");
	if (!results_fuc) {
		finishCommunication();
		throw std::exception("Error: object/function getFeedback not Found");
	}
	if (!PyCallable_Check(results_fuc)) {
		finishCommunication();
		throw std::exception("Error: object/function getFeedback not callable");
	}

	//call the init function in python

	PyObject* init_func, * returnedValue;
	init_func = PyObject_GetAttrString(this->module, "init");
	if (!init_func) {
		finishCommunication();
		throw;
	}
	if (!PyCallable_Check(init_func)) {
		finishCommunication();
		throw;
	}

	PyObject* args = PyTuple_New(3);
	PyTuple_SetItem(args, 0, PyLong_FromLong(input_size));
	PyTuple_SetItem(args, 1, PyLong_FromLong(output_size));
	PyTuple_SetItem(args, 2, PyFloat_FromDouble(learning_rate));

	returnedValue = PyObject_Call(init_func, args, nullptr);
	if (!returnedValue) {
		finishCommunication();
		throw std::exception("Error: Error occured. PyObject* returned by init was null");
	}
	//std::cout << _PyUnicode_AsString(PyObject_Type(returnedValue));
	if (!PyBool_Check(returnedValue)) {
		finishCommunication();
		throw std::exception("Exception: Invalid retunrd type. Returned type was no of type bool");
	}

	int compare_result = PyObject_RichCompareBool(returnedValue, Py_True, Py_EQ);
	if (compare_result != 1) { //!=1 pt ca pote returna -1 la eroare
		finishCommunication();
		throw std::exception(("Error: python call for init result in an error, value "s + std::to_string(compare_result) + " was returned by comparison function").c_str());
	}
	Py_DECREF(returnedValue);
	Py_DECREF(init_func);
}

pyCommunication::~pyCommunication()
{
	if(is_running) finishCommunication();
}

void pyCommunication::finishCommunication()
{
	if (!is_running) return;
	is_running = false;
	//to be sure that all memory is corectly dealocated, i decrement the counter before ending the session
	//this may be unnecessary, but at this point I don't know very well how the Py C library work
	Py_XDECREF(func_arg);
	Py_XDECREF(module);
	Py_XDECREF(call_func);
	Py_XDECREF(results_fuc);
	Py_Finalize();
}


void pyCommunication::init_parser(int elementCount)
{
	Py_CLEAR(func_arg);
	func_arg = PyTuple_New(elementCount);
	CHECK_CREATED(func_arg);
	crt_element = 0;
}

void pyCommunication::addElemToTuple(PyObject* tuple_entry) {
	PyTuple_SetItem(func_arg, crt_element, tuple_entry);
	crt_element++;
}

void pyCommunication::parse_double(const char* name, const double value)
{
	PyObject* tuple_entry, *tmp;
	tuple_entry = PyTuple_New(2);
	CHECK_CREATED(tuple_entry);

	tmp = PyUnicode_FromString(name);
	CHECK_CREATED(tmp);
	PyTuple_SetItem(tuple_entry, 0, tmp);
	tmp = PyFloat_FromDouble(value);
	CHECK_CREATED(tmp);
	PyTuple_SetItem(tuple_entry, 1, tmp);

	addElemToTuple(tuple_entry);
}

void pyCommunication::parse_double_array(const char* name, const std::vector<double>& value)
{
	PyObject *tuple_entry, *elements, *tmp;
	tuple_entry = PyTuple_New(2);
	CHECK_CREATED(tuple_entry);
	tmp = PyUnicode_FromString(name);
	CHECK_CREATED(tmp);
	PyTuple_SetItem(tuple_entry, 0, tmp);
	
	elements = PyTuple_New(value.size());
	CHECK_CREATED(elements);
	for (int i = 0; i < value.size(); i++) {
		tmp = PyFloat_FromDouble(value[i]);
		CHECK_CREATED(tmp);
		PyTuple_SetItem(elements, i, tmp);
	}
		

	PyTuple_SetItem(tuple_entry, 1, elements);

	addElemToTuple(tuple_entry);
}

default_ReturnedValueFromStript pyCommunication::call()
{
	default_ReturnedValueFromStript returned_value = { 0,0,0 };

	PyObject* arg = PyTuple_New(1);
	CHECK_CREATED(arg);
	PyTuple_SetItem(arg, 0, func_arg);
	Py_XINCREF(func_arg);

	PyObject* returned_object = PyObject_Call(this->call_func, arg, NULL);


	if (!returned_object) {
		PyErr_Print();
		std::cout << std::endl;
		throw std::exception();
	}
	else if(PyDict_Check(returned_object)){
		returned_value.forward = (float)PyFloat_AsDouble(PyDict_GetItemString(returned_object, "forward"));
		returned_value.up = (float)PyFloat_AsDouble(PyDict_GetItemString(returned_object, "upwards"));
		returned_value.rotation_angle = (float)PyFloat_AsDouble(PyDict_GetItemString(returned_object, "rotation angle"));
	}
	else {
		throw std::exception("returned type not a dictionary");
	}
	Py_XDECREF(returned_object);
	Py_XDECREF(arg);

	return returned_value;
}

void pyCommunication::give_feedback(const default_FeedbackType & newParams)
{
	PyObject* tuple;
	tuple = PyTuple_New(8);
	CHECK_CREATED(tuple);
	PyTuple_SetItem(tuple, 0, func_arg);
	PyTuple_SetItem(tuple, 1, PyFloat_FromDouble(newParams.new_pos[0]));
	PyTuple_SetItem(tuple, 2, PyFloat_FromDouble(newParams.new_pos[1]));
	PyTuple_SetItem(tuple, 3, PyFloat_FromDouble(newParams.new_pos[2]));
	PyTuple_SetItem(tuple, 4, PyBool_FromLong(newParams.hasCollide));
	PyTuple_SetItem(tuple, 5, PyFloat_FromDouble(newParams.destination[0]));
	PyTuple_SetItem(tuple, 6, PyFloat_FromDouble(newParams.destination[1]));
	PyTuple_SetItem(tuple, 7, PyFloat_FromDouble(newParams.destination[2]));

	Py_XDECREF(PyObject_Call(this->results_fuc, tuple, nullptr));
	Py_XDECREF(tuple);
}



#undef CHECK_CREATED