#include <cmath>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

#include "stack.h"

using namespace std;

bool pop_value(Stack* stack, int& value)
{
    if (stack_empty(stack))
    {
        return false;
    }

    value = stack_get(stack);
    stack_pop(stack);
    return true;
}

bool read_number(const string& command, int& value)
{
    if (command.empty())
    {
        return false;
    }

    size_t start = command[0] == '-' ? 1 : 0;
    if (start == command.size())
    {
        return false;
    }

    for (size_t i = start; i < command.size(); ++i)
    {
        if (command[i] < '0' || command[i] > '9')
        {
            return false;
        }
    }

    try
    {
        value = stoi(command);
    }
    catch (const exception&)
    {
        return false;
    }

    return true;
}

int fail(Stack* stack, const string& message)
{
    cerr << message << '\n';
    stack_delete(stack);
    return 1;
}

int main(int argc, char* argv[])
{
    if (argc != 3)
    {
        cerr << "Usage: Lab2CPP <script> <input>\n";
        return 1;
    }

    ifstream script(argv[1]);
    ifstream input(argv[2]);

    if (!script || !input)
    {
        cerr << "Cannot open an input file\n";
        return 1;
    }

    vector<string> commands;
    string command;
    while (script >> command)
    {
        commands.push_back(command);
    }

    Stack* stack = stack_create();
    int repeatCount = 0;
    size_t repeatStart = 0;
    bool repeatIsSet = false;
    size_t conditionDepth = 0;

    size_t i = 0;
    while (i < commands.size())
    {
        command = commands[i];

        int number = 0;
        if (read_number(command, number))
        {
            stack_push(stack, number);
        }
        else if (command == "get")
        {
            if (!(input >> number))
            {
                return fail(stack, "Invalid input");
            }
            stack_push(stack, number);
        }
        else if (command == "peek")
        {
            if (stack_empty(stack))
            {
                return fail(stack, "peek requires one value");
            }
            cout << stack_get(stack) << ' ';
        }
        else if (command == "add" || command == "sub" ||
                 command == "mul" || command == "div")
        {
            int top = 0;
            int bottom = 0;
            if (!pop_value(stack, top) || !pop_value(stack, bottom))
            {
                return fail(stack, command + " requires two values");
            }

            if (command == "add")
            {
                stack_push(stack, top + bottom);
            }
            else if (command == "sub")
            {
                stack_push(stack, top - bottom);
            }
            else if (command == "mul")
            {
                stack_push(stack, top * bottom);
            }
            else
            {
                if (bottom == 0)
                {
                    return fail(stack, "Division by zero");
                }
                stack_push(stack, top / bottom);
            }
        }
        else if (command == "sqrt" || command == "sq")
        {
            int value = 0;
            if (!pop_value(stack, value))
            {
                return fail(stack, command + " requires one value");
            }

            if (command == "sqrt")
            {
                if (value < 0)
                {
                    return fail(stack, "Square root of a negative value");
                }
                stack_push(stack, static_cast<int>(sqrt(value)));
            }
            else
            {
                stack_push(stack, value * value);
            }
        }
        else if (command == "cond")
        {
            int top = 0;
            if (!pop_value(stack, top) || stack_empty(stack))
            {
                return fail(stack, "cond requires two values");
            }

            int bottom = stack_get(stack);
            if (top == bottom)
            {
                stack_pop(stack);
                ++conditionDepth;
            }
            else
            {
                size_t nestedDepth = 1;
                while (nestedDepth > 0)
                {
                    ++i;
                    if (i >= commands.size())
                    {
                        return fail(stack, "cond has no matching end");
                    }
                    if (commands[i] == "cond")
                    {
                        ++nestedDepth;
                    }
                    else if (commands[i] == "end")
                    {
                        --nestedDepth;
                    }
                }
            }
        }
        else if (command == "end")
        {
            if (conditionDepth == 0)
            {
                return fail(stack, "end has no matching cond");
            }
            --conditionDepth;
        }
        else if (command == "setr")
        {
            if (!pop_value(stack, repeatCount))
            {
                return fail(stack, "setr requires one value");
            }
            repeatStart = i + 1;
            repeatIsSet = true;
        }
        else if (command == "repeat")
        {
            if (!repeatIsSet)
            {
                return fail(stack, "repeat has no matching setr");
            }

            if (repeatCount > 1)
            {
                --repeatCount;
                i = repeatStart;
                continue;
            }
            repeatCount = 0;
        }
        else
        {
            return fail(stack, "Unknown command: " + command);
        }

        ++i;
    }

    if (conditionDepth != 0)
    {
        return fail(stack, "cond has no matching end");
    }

    cout << '\n';
    stack_delete(stack);
    return 0;
}
