/* 
   gotApproval               yes/no question

   Inputs:
        cptr    =       pointer to question
        defaultAnswer   =       default answer
   Outputs:
        result  =       true if yes, false if no
*/
#include <iostream>
#include <string>

bool gotApproval(std::string question, bool defaultAnswer)
{
    std::string answer;

    std::cout << question;
    std::cin >> answer;
    
    if (answer.empty())
        return defaultAnswer;

    if (answer.starts_with ('y') || answer.starts_with ('Y'))
    return true;

    return false;
}