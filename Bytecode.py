class Bytecode:
    def PUSH(self, value, stack):
        return stack.append(value)
    
    def POP(self, stack):
        if stack == dict():
            if not stack:
                raise IndexError("POP from empty stack")
            return stack.pop(stack[-1])
        if stack == list():
            if not stack:
                raise IndexError("POP from empty stack")
            return stack.pop()
    
    def STORE(self, var_name, stack, variables):
        bytecode = Bytecode()
        value = bytecode.POP(stack)
        variables[var_name] = value
        return variables
    
    def ADD(self, stack):
        if len(stack) < 2:
            raise IndexError("Not enough values on stack to perform ADD")
        bytecode = Bytecode()
        b = bytecode.POP(stack)
        a = bytecode.POP(stack)
        stack.append(a + b)
        return stack
    
    def SUB(self, stack):
        if len(stack) < 2:
            raise IndexError("Not enough values on stack to perform SUB")
        bytecode = Bytecode()
        b = bytecode.POP(stack)
        a = bytecode.POP(stack)
        stack.append(a - b)
        return stack
    
    def MUL(self, stack):
        if len(stack) < 2:
            raise IndexError("Not enough values on stack to perform MUL")
        bytecode = Bytecode()
        b = bytecode.POP(stack)
        a = bytecode.POP(stack)
        stack.append(a * b)
        return stack
    
    def DIV(self, stack):
        if len(stack) < 2:
            raise IndexError("Not enough values on stack to perform DIV")
        bytecode = Bytecode()
        b = bytecode.POP(stack)
        a = bytecode.POP(stack)
        if b == 0:
            raise ZeroDivisionError("division by zero")
        stack.append(a / b)
        return stack

    def CALL(self, func, stack):
        bytecode = Bytecode()
        if func == "PRINT":
            value = bytecode.POP(stack)
            print(value)
        elif func == "DUP":
            value = stack[-1]
            stack.append(value)
        elif func == "SWAP":
            if len(stack) < 2:
                raise IndexError("Not enough values on stack to perform SWAP")
            stack[-1], stack[-2] = stack[-2], stack[-1]
        else:
            raise ValueError(f"Unknown function: {func}")