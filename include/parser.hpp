#include "lexer.hpp"

namespace Aardvark {
  using namespace std;

  unordered_map<string, int> PRECEDENCE = {
    { "=", 1 },

    { "||", 4 },
    { "&&", 5 },

    { "<", 7 }, { ">", 7 }, { "<=", 7 }, { ">=", 7 }, { "==", 7 },

    { "+", 10 }, { "-", 10 },
    { "*", 20 }, { "/", 20 }, { "%", 20 }
  };

  enum class ExprTypes { // access it by ExprTypes::Whatever
    None,
    While,
    If,
    Int,
    Float,
    String,
    Variable,
    Identifier,
    Assign,
    Binary,
    Scope,
    Function,
    FunctionCall,
    Class,
    Return
  };

  vector<string> ExprStrings = {
    "None",
    "While",
    "If",
    "Int",
    "Float",
    "String",
    "Variable",
    "Identifier",
    "Assign",
    "Binary",
    "Scope",
    "Function",
    "FunctionCall",
    "Class",
    "Return"
  };

  // For debugging later
  string getExprString(ExprTypes type) {
    return ExprStrings[((int)type)];
  }

  template<typename T>
  bool vincludes(vector<T> arr, T value) {
    return find(arr.begin(), arr.end(), value) != arr.end();
  };

  class Expression {
    public:
    ExprTypes type;
    Token value;

    Expression* left;
    Token op;
    Expression* right;

    vector<Expression*> block;
    vector<Expression*> args;

    Token identifier;
    Expression* dotOp; // dot operation for like properties and stuff

    Expression(Token value): type(ExprTypes::None), value(value) {};
    Expression(ExprTypes type, Token value): type(type), value(value) {};
  };

  class Parser {
    public:
    vector<Token> tokens;

    Token curTok;
    int pos = 0;

    Expression* ast;

    Parser(vector<Token> tokens): tokens(tokens) {};

    Token advance(int amt = 1) {
      if (pos + amt >= tokens.size()) return curTok = Token();
      pos += amt;

      return curTok = tokens[pos];
    }

    Token peek(int amt = 1) {
      if (pos + amt >= tokens.size()) return Token();

      return tokens[pos + amt];
    }

    bool isEOF() {
      return curTok.type == "EOF";
    }

    bool isType(string type, string value, Token peeked) {
      return peeked.type == type && peeked.getString() == value;
    }
    bool isType(string type, Token peeked) {
      return peeked.type == type;
    }
    bool isType(string type, string value) {
      return curTok.type == type && curTok.getString() == value;
    }
    bool isType(string type) {
      return curTok.type == type;
    }

    bool isIgnore(Token tok) {
      return tok.type == "Delimiter" && tok.getString() == ";";
    }

		void skipOverVal(string val, Token tok) {
      if (tok.getString() != val) throw SyntaxError(tok.getString());

      advance();
    }

    void skipOver(string type, string val) {
      if (curTok.type != type || curTok.getString() != val) throw SyntaxError(curTok.getString());

      advance();
    }
    void skipOver(string type) {
      if (curTok.type != type) throw SyntaxError(curTok.getString());

      advance();
    }

    void skipOver(string type, Token tok) {
      if (tok.type != type) throw SyntaxError(tok.getString());

      advance();
    }
    void skipOver(string type, string val, Token tok) {
      if (tok.type != type || tok.getString() != val) throw SyntaxError(tok.getString());

      advance();
    }

		bool nonCallabes(Expression* expression) {
      return (
        expression->type != ExprTypes::Function
        && expression->type != ExprTypes::If
      );
    }

    // Parse delimtiers from start to end with expressions in a vector
    vector<Expression*> pDelimiters(string start, string end, string separator) {
      vector<Expression*> values = {};
      bool isFirst = true;

      skipOverVal(start, curTok);

      while (!isEOF()) {
        if (isType("Delimiter", end)) {
          break;
        } else if (isFirst) {
          isFirst = false;
        } else {
          skipOverVal(separator, curTok);
        }

        Expression* val = pExpression();
        values.push_back(val);
      }
      skipOverVal(end, curTok);

      return values;
    }

		// Parse any prop / dot operations
		// probably should change how this works
		bool pDotOp(Expression* exp) {
			// TODO
			if (isType("Delimiter", ".")) {
        advance();

				if (!isType("Identifier"))
					throw SyntaxError(curTok.getString()); // Meaningful errors soon

				exp->identifier = curTok;
        exp->dotOp = pExpression();
        return true;
      }

      return false;
		}

    Expression* isCall(Expression* expression) {
      return isType("Delimiter", "(", peek()) && nonCallables(expression) ? pCall(expression);
    }

    Expression* pCall(Expression* expr) {
      // TODO: Parse function calls here
    }

    Expression* pBinary(Expression* left, int prec) {
      // TODO: Parse Binary Operations or Assignment
      Token op = curTok;

      if (isType("Operator")) {
        string opvalue = op.getString();
        int newPrec = PRECEDENCE[opvalue];

        if (newPrec > prec) {
          advance();

          vector<string> assignments = {
            "=",
          };

          ExprTypes type = vincludes<string>(assignments, opvalue)
            ? ExprTypes::Assign
            : ExprTypes::Binary;
        }
      }
    }

    Expression* pAll() {
      // TODO: Complete this function
    };

    Expression* pExpression() {
      // this automagically helps us with formatting the ast correctly
      return isCall(pBinary(isCall(pAll()), 0));
    };

    Expression* parse() {
      asat = new Expression(ExprTypes::Scope, Token("_TOP_", true)); // Setting up the top scope since adk doesn't have a main func
      curTok = tokens[0];

      while (!curTok.isNull() && !isEOF()) {
        Expression* expr = pExpression();
        ast->block.push_back(expr);

        if (!curTok.isNull() && !isEOF()) skipOver("Delimiter", ";");
      }

      return ast;
    }
  };
};