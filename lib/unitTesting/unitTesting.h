#ifndef UNIT_TESTING_H
#define UNIT_TESTING_H

// Runs through a set of hardcoded tests, and if any of them fails, raises a
// critical error.
// Returns the number of failures
// takes, bool, set to true to crash when a test fails
int runAllTests(bool crashUponFail = false);



#endif  // UNIT_TESTING_H
