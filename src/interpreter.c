#include "interpreter.h"

#include "error.h"
#include "nob.h"

static Instruction currentInstruction;

static void doPush(ValueStack * stack, Value v)
{
	nob_da_append(stack, v);
}

static Value doPop(ValueStack * stack)
{
	if (stack->count < 1) {
		reportError(currentInstruction.token.filePath, currentInstruction.token.lineNum, currentInstruction.token.colNum, ERROR_SEGFAULT_POP_FROM_EMPTY_STACK);
		exit(1);
	}
	
	Value v = nob_da_last(stack);
	stack->count--;
	return v;
}

static Value doPlus(ValueStack * stack)
{
    Value a = doPop(stack);
    Value b = doPop(stack);
    
    switch (a.type) {
    case I32:
        switch (b.type) {
        case I32:
            return i32Value(a.i32 + b.i32);
        default:
            assert(false && "Unreachable");
        break;
        }
        break;
    default:
        assert(false && "Unreachable");
        break;
    }
}

static Value doMinus(ValueStack * stack)
{
    Value a = doPop(stack);
    Value b = doPop(stack);
    
	switch (a.type) {
	case I32:
		switch (b.type) {
		case I32:
			return i32Value(a.i32 - b.i32);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	default:
		assert(false && "Unreachable");
		break;
	}
}

static Value doMultiply(ValueStack * stack)
{
    Value a = doPop(stack);
    Value b = doPop(stack);
    
	switch (a.type) {
	case I32:
		switch (b.type) {
		case I32:
			return i32Value(a.i32 * b.i32);
		default:
			assert(false && "Unreachable");
			break;
		}
    break;
    default:
        assert(false && "Unreachable");
        break;
    }
}

static Value doDivide(ValueStack * stack)
{
    Value a = doPop(stack);
    Value b = doPop(stack);
    
	switch (a.type) {
	case I32:
		switch (b.type) {
		case I32:
			return i32Value(a.i32 / b.i32);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	default:
		assert(false && "Unreachable");
		break;
	}

	return (Value) {0};
}

static void doDump(ValueStack * stack)
{
    Value v = doPop(stack);
	switch (v.type) {
	case I32:
		printf("%d\n", v.i32);
		break;
	default:
		assert(false && "Unreachable");
		break;
	}
}

static Value doEqual(ValueStack * stack)
{
    Value a = doPop(stack);
    Value b = doPop(stack);
    
	switch (a.type) {
	case I32:
		switch (b.type) {
		case I32:
			return i32Value(a.i32 == b.i32);
		default:
			assert(false && "Unreachable");
			break;
		}
		break;
	default:
		assert(false && "Unreachable");
		break;
	}
}

static void doIf(ValueStack * stack, size_t * ip)
{
    Value v = doPop(stack);
    
	if (!v.i32) *ip = currentInstruction.value.i32;
}

static void doElse(size_t * ip)
{
	*ip = currentInstruction.value.i32;
}

static void doEnd(size_t * ip)
{
	if (currentInstruction.value.i32) *ip = currentInstruction.value.i32;
}

static Value doGt(ValueStack * stack)
{
    Value a = doPop(stack);
    Value b = doPop(stack);
    
	switch (a.type) {
	case I32:
		switch (b.type) {
		case I32:
			return i32Value(a.i32 > b.i32);
	    default:
	        assert(false && "Unreachable");
	    }
	    break;
	default:
		assert(false && "Unreachable");
	}
}

static void doDup(ValueStack * stack)
{
    Value v = doPop(stack);
    
	doPush(stack, v);
	doPush(stack, v);
}

static void doWhile()
{
	
}

static void doDo(ValueStack * stack, size_t * ip)
{
    Value v = doPop(stack);
	if (!v.i32) *ip = currentInstruction.value.i32;
}

static Value doLt(ValueStack * stack)
{
    Value a = doPop(stack);
    Value b = doPop(stack);
    
	switch (a.type) {
	case I32:
		switch (b.type) {
		case I32:
			return i32Value(a.i32 < b.i32);
	    default:
			assert(false && "Unreachable");
			break;
		}
		break;
	default:
		assert(false && "Unreachable");
		break;
	}
}

static void interpretInstruction(ValueStack * stack, size_t * ip)
{
	size_t originalIp = *ip;
	
	switch (currentInstruction.token.type) {
		case TOK_PUSH:
			doPush(stack, currentInstruction.value);
			break;
		case TOK_PLUS:
			doPush(stack, doPlus(stack));
			break;
		case TOK_MINUS:
			doPush(stack, doMinus(stack));
			break;
		case TOK_MULTIPLY:
			doPush(stack, doMultiply(stack));
			break;
		case TOK_DIVIDE:
			doPush(stack, doDivide(stack));
			break;
		case TOK_DUMP:
			doDump(stack);
			break;
		case TOK_EQUAL:
			doPush(stack, doEqual(stack));
			break;
		case TOK_IF:
			doIf(stack, ip);
			break;
		case TOK_ELSE:
			doElse(ip);
			break;
		case TOK_END:
			doEnd(ip);
			break;
		case TOK_DUP:
			doDup(stack);
			break;
		case TOK_GT:
			doPush(stack, doGt(stack));
			break;
		case TOK_WHILE:
			doWhile();
			break;
		case TOK_DO:
			doDo(stack, ip);
			break;
		case TOK_LT:
			doPush(stack, doLt(stack));
			break;
		default:
			assert(false && "Unreachable");
			break;
	}
	
	if (*ip == originalIp) *ip += 1;
}

void interpretProgram(InstructionArray * instructions)
{
	ValueStack stack = {0};
	size_t ip = 0;
	while (ip < instructions->count) {
	    currentInstruction = instructions->items[ip];
		interpretInstruction(&stack, &ip);
	}
	nob_da_free(stack);
}
