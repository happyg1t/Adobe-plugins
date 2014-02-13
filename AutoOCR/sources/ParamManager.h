#ifndef _ParamManager_
#define _ParamManager_
#include "ASExpT.h"

#include <string>

using namespace std;

/** provides services for managing a parameter list. This class allows a string 
	parameterlist to be enumerated, passing back the paramter as its true type
*/
class ParamManager {
public:
	ParamManager(){};

	/** dtor */
	virtual ~ParamManager();
	/** we don't actually do the dialog for PDFL, but we use the same hook... */
	ASInt32 doDialog(const char * name, const char * desc, const char * defparams,const char * params);

		/** we don't actually do the dialog for PDFL, but we use the same hook... */
	ASInt32 getParams(const char * name, const char * desc, const char * defparams,const char * params);

	/** returns the number of parameters in the string. With every "get" call, this number decrements by one 
		@return number of parameters
	*/
	ASInt32 getNumberParams();
	/** extracts the next parameter from the list as an integer.
		@param param OUT the value of the parameter
		@returns true if the next parameter is a valid integer, false if there are no parameters left, or if the
		next parameter is not a valid integer. Currently not implemented (always returns true)
	*/
	ASBool getNextParamAsInt(ASInt32 &param);
	/** extracts the next parameter from the list as a string.
		@param param OUT the value of the parameter
		@returns true if the next parameter is a valid string, false if there are no parameters left, or if the
		next parameter is not a valid string. Currently not implemented (always returns true)
	*/
	ASBool getNextParamAsString(string &param);
	/** extracts the next parameter from the list as an ASFixed.
		@param param OUT the value of the parameter
		@returns true if the next parameter is a valid ASFixed, false if there are no parameters left, or if the
		next parameter is not a valid ASFixed. Currently not implemented (always returns true)
	*/
	ASBool getNextParamAsFixed(ASFixed &param);
	/** extracts the next param, converting it from a textual representation of heximal data into the memory ptr passed in 
		so, passing in 0000001200000008 would result in the memory being populated with the int 12, the int 8 and it would
		set the numOfEntries to 2.
		Caller is responsible for freeing dataEntered using ASfree.
	*/
	ASBool getNextParamAsHexData(void **dataEntered, ASInt32 &numEntries);
	/** return the whole parameter list as a single string */
	string getParamList(){return fParams;};
private:
	/** we maintain a queue of strings for the parameter list */
	struct SNode{
		string param;
		SNode * next;
	};
	SNode * head;
	SNode * tail;
	void push(string tmpVal);
	string pop();
	/** we maintain the parameters as a string, this allows them to be cached elsewhere*/
	string fParams;
};
#endif // _ParamManager_

