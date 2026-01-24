from Bytecode import Bytecode

stack = []
variables = {}
bytecode = Bytecode()
bytecode.PUSH(10, stack)
print(stack)
print(bytecode.STORE("a", stack, variables))
print(stack)
bytecode.CALL("PRINT", variables["a"])
bytecode.PUSH(20, stack)
print(stack)
bytecode.CALL("PRINT", bytecode.ADD(stack))