#ifndef INPUT_HANDLER_H_
#define INPUT_HANDLER_H_



// Creates a C-string by reading byte by byte through stdin. On success, pointer
// to the string is placed in "line_ptr_ret", which the caller owns. 
// Arguments:
// - line_ptr_ret: a return parameter that will store a pointer to 
//   the allocated C-string
// Returns:
// - true if successful (eg there was no issue reading the bytes from 
//   stdin), false otherwise. 
bool ReadLine(char** line_ptr_ret);

#endif  // INPUT_HANDLER_H_