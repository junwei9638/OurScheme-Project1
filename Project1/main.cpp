# include <ctype.h>
# include <stdio.h>
# include <stdlib.h>
# include <math.h>
# include <cctype>
# include <iostream>
# include <iomanip>
# include <string>
# include <vector>
# include <sstream>

using namespace std;

struct  Token {
  string tokenName  ;
  int typeNum  ;
  int tokenColumn ;
  int tokenLine ;
}; // TokenType


struct TokenTree {
  Token * leftToken  ;
  Token * rightToken ;
  TokenTree * leftNode  ;
  TokenTree * rightNode  ;
  TokenTree * backNode  ;
}; // TokenType


enum Type {
  INT, STRING, DOT, FLOAT, NIL, T, QUOTE, SYMBOL, LEFTPAREN, RIGHTPAREN, NO_TYPE
}; // Type

enum Error {
  LEFT_ERROR, RIGHT_ERROR, CLOSE_ERROR, EOF_ERROR, NO_ERROR, NOT_S_EXP_ERROR
}; // Error

vector<Token> gTokens;

int gLine = 1 ;
int gColumn = 0 ;

bool gIsEnd = false;
int gAtomType = 0 ;

string gErrorMsgName = "\0" ;
int gErrorMsgType = NO_ERROR ;
int gErrorLine = 0 ;
int gErrorColumn = 0 ;

// ------------------Setting Function--------------------- //
void InitialLineColumn() {
  gLine = 1 ;
  gColumn = 0 ;
} // InitialLineColumn()

void ClearSpaceAndOneLine() {               // read space and "ONE" Line
  char peek = cin.peek() ;
  InitialLineColumn() ;
  
  while ( peek == ' ' || peek == '\t' ) {
    cin.get() ;
    gColumn++ ;
    peek = cin.peek() ;
  } // while
  
  if ( peek == '\n' ) {                    // if == endl -> initial line and column
    InitialLineColumn() ;                  // else record the line and column
    cin.get() ;
  } // if
  
  if ( peek == ';' ) {                     // comment case
    while ( peek != '\n' && peek != EOF ) {
      cin.get() ;
      peek = cin.peek() ;
    } // while
    
    if ( peek == '\n' ) cin.get() ;
    InitialLineColumn() ;                  // get endl
  } // if

} // ClearSpaceAndOneLine()

void ClearInput() {
  char peek = cin.peek() ;
  while ( peek != '\n' && peek != EOF ) {
    cin.get();
    peek = cin.peek() ;
  } // while
  
  if ( peek == '\n' ) cin.get() ;
} // ClearInput()

void SetErrorMsg( int errorType, string errorToken, int line, int column ) {
  gErrorMsgType = errorType ;
  gErrorMsgName = errorToken ;
  gErrorLine = line ;
  gErrorColumn = column ;
} // SetErrorMsg()

void GlobalVariableReset() {
  gTokens.clear() ;
  gErrorLine = 1;
  gErrorColumn = 0;
  gErrorMsgType = NO_ERROR ;
  gErrorMsgName = "\0" ;
  gAtomType = NO_TYPE ;
} // GlobalVariableReset()

bool ExitDetect() {

  int nilExit = -1 ;
  int exitNil = -1 ;
  string tokenString = "\0" ;
  
  for ( int i = 0 ; i < gTokens.size() ; i++ )
    tokenString += gTokens[i].tokenName ;
  
  nilExit = ( int ) tokenString.find( "(nil.exit)" ) ;
  exitNil = ( int ) tokenString.find( "(exit.nil)" ) ;
  
  if ( ( tokenString == "(exit)" && gTokens.size() == 3 ) ||
       ( nilExit != -1 && gTokens.size() == 5 ) ||
       ( exitNil != -1 && gTokens.size() == 5 ) ) {
    gIsEnd = true ;
    return true ;
  } // if

  
  return false ;
} // ExitDetect()


// ------------------Get Token--------------------- //
  
string StringProcess() {
  string inputString = "\0" ;
  bool closeQuote = false ;
  char ch = cin.get() ;
  gColumn ++ ;
  inputString += ch ;
  char peek = cin.peek() ;
  gAtomType = STRING ;
  
  while ( closeQuote == false && peek != '\n' && peek != EOF ) {
    
    ch = cin.get() ;
    gColumn ++ ;
    peek = cin.peek() ;
    
    if ( ch == '"' ) closeQuote = true ;
    
    if ( ch == '\\' && peek == '\\' ) {              // double slash
      cin.get();
      gColumn ++ ;
      peek = cin.peek() ;
      inputString += '\\' ;
    } // if
    
    else if ( ch == '\\' && peek == 'n' ) {           // \n case
      cin.get() ;
      gColumn ++ ;
      peek = cin.peek() ;
      inputString += '\n' ;
    } // if
    
    else if ( ch == '\\' && peek == 't' ) {          // \t case
      cin.get() ;
      gColumn ++ ;
      peek = cin.peek() ;
      inputString += '\t' ;
    } // if
    
    else if ( ch == '\\' && peek == '"' ) {          // \" case
      cin.get() ;
      gColumn ++ ;
      peek = cin.peek() ;
      inputString += '"' ;
    } // if
    
    else inputString += ch ;
    
  } // while
  
  
  if ( closeQuote == false ) {
    if ( peek == EOF )
      SetErrorMsg( EOF_ERROR, "\"", 0, 0 ) ;
    else if ( peek == '\n' )
      SetErrorMsg( CLOSE_ERROR, "\"", gLine, gColumn+1 ) ;
    return "\0" ;
  } // if                                        // no closing quote
  
  return inputString ;
} // StringProcess()

string NumProcess( string atomExp ) {
  
  if ( atomExp[0] == '+' ) atomExp = atomExp.substr( 1, atomExp.length() - 1 ) ;  // '+' case
  if ( atomExp[0] == '.'  ) atomExp = "0" + atomExp ;                             // .xxx case
  if ( atomExp[0] == '-' && atomExp[1] == '.' ) atomExp.insert( 1, "0" ) ;        // -.xxx case
  
  int dot = ( int ) atomExp.find( '.' );
  if ( dot != atomExp.npos ) gAtomType = FLOAT ;
  else gAtomType = INT ;
  return atomExp ;
} // NumProcess()

string AtomAnalyze( string atomExp ) {
  bool isNum = false ;
  bool isSymbol = false ;
  int signBit = 0 ;
  int digitBit = 0 ;
  int dotBit = 0;
  if ( atomExp == "#t" || atomExp == "t" ) {
    atomExp = "#t" ;
    gAtomType = T ;
  } // if
  
  else if ( atomExp == "nil" || atomExp == "#f" ) {
    atomExp = "nil" ;
    gAtomType = NIL ;
  } // if
  
  else if ( atomExp == "'" || atomExp == "quote"  ) {
    gAtomType = QUOTE ;
  } // if
  
  else if ( atomExp == "." ) {
    atomExp = "." ;
    gAtomType = DOT ;
  } // if
  
  else {
    
    for ( int i = 0 ; i < atomExp.length() ; i ++ ) {
      if ( ( ( int ) atomExp[i] >= 48 && ( int ) atomExp[i] <= 57 ) ||
           atomExp[i] == '+' || atomExp[i] == '-' ||  atomExp[i] == '.' ) {
        if ( atomExp[i] == '+' || atomExp[i] == '-' ) signBit ++ ;
        if ( ( int ) atomExp[i] >= 48 && ( int ) atomExp[i] <= 57 ) digitBit ++ ;
        if ( atomExp[i] == '.' ) dotBit ++ ;
        isNum = true;
      } // if
      
      else isSymbol = true;
    } // for                                                           // Num Judge
    
    if ( signBit > 1 || digitBit == 0 || dotBit > 1 ) isNum = false ;
    
    gAtomType = SYMBOL ;
    if ( isNum && !isSymbol ) atomExp = NumProcess( atomExp );
  } // else

  
  return  atomExp;
} // AtomAnalyze()

string GetAtom( ) {
  string atomExp = "\0" ;
  char ch = '\0' ;
  char peek = cin.peek() ;
  while ( peek != '\n' && peek != '"' &&
          peek != ' ' && peek != ';' &&
          peek != '(' && peek != ')' &&
          peek != '\0' && peek != EOF &&
          peek !=  '\'' && peek !=  '\t' &&
          peek !=  '\r' ) {
    ch = cin.get() ;
    gColumn ++ ;
    atomExp = atomExp + ch;
    peek = cin.peek() ;
  } // while
  
  atomExp = AtomAnalyze( atomExp );
  
  return atomExp ;
} // GetAtom()

char GetChar( ) {
  char peek = '\0' ;
  peek = cin.peek() ;
  while ( peek == ' ' || peek == '\t' || peek == '\n' || peek == ';'  ) {
    if ( peek == ';' ) {
      while ( peek != '\n' && peek != EOF ) {
        cin.get() ;
        gColumn ++ ;
        peek = cin.peek() ;
      } // while
    } // if
    
    else if ( peek == '\n' ) {
      cin.get() ;
      gLine++ ;
      gColumn = 0 ;
    } // if
    
    else if ( peek == ' ' || peek == '\t' ) {
      cin.get() ;
      gColumn ++ ;
    } // if
    
    
    peek = cin.peek() ;
  } // while
  
  return peek ;
} // GetChar()

bool GetToken() {
  Token token;
  char peek = GetChar() ;

  token.tokenName = "\0"  ;
  token.typeNum = NO_TYPE  ;
  token.tokenColumn = 0 ;
  token.tokenLine = 0 ;
  
  if ( peek == EOF ) {
    SetErrorMsg( EOF_ERROR, " ", 0, 0  ) ;
    gIsEnd = true ;
    return false;
  } // if
  
  else {
    
    if ( peek == '(' ) {
      token.tokenName = cin.get() ;
      gColumn ++ ;
      token.tokenColumn = gColumn ;
      token.tokenLine = gLine ;
      peek = GetChar() ;
      
      if ( peek == ')' ) {
        token.tokenName = "nil" ;
        cin.get() ;
        gColumn ++ ;
        gAtomType = NIL ;
      } // if                                                   // () case
      
      else
        gAtomType = LEFTPAREN;
    } // if                                         // left praen
    
    else if ( peek == ')' ) {
      token.tokenName = cin.get() ;
      gColumn ++ ;
      token.tokenColumn = gColumn ;
      token.tokenLine = gLine ;
      gAtomType = RIGHTPAREN;
    } // if             // right paren
    
    else if ( peek == '"' ) {
      token.tokenColumn = gColumn + 1 ;
      token.tokenLine = gLine ;
      token.tokenName = StringProcess() ;
      if ( token.tokenName == "\0" ) return false ;
    } // if                                        // string
    
    
    else if ( peek == '\'' ) {
      token.tokenName = cin.get() ;
      gColumn ++ ;
      token.tokenColumn = gColumn ;
      token.tokenLine = gLine ;
      gAtomType = QUOTE;
    } // if
    
    else {
      token.tokenColumn = gColumn + 1 ;
      token.tokenLine = gLine ;
      token.tokenName = GetAtom() ;
    } // else
    
    token.typeNum = gAtomType ;
    gTokens.push_back( token ) ;
    
    return true;
  } // else
  
} // GetToken()

// ------------------Tree Build--------------------- //

bool IsAtom( int typeNum ) {
  if ( typeNum == SYMBOL || typeNum == INT ||
       typeNum == FLOAT || typeNum == STRING ||
       typeNum == NIL || typeNum == T )
    return true ;
  
  else return false ;
} // IsAtom()



bool SyntaxChecker() {
  if ( IsAtom( gTokens.back().typeNum ) || gTokens.back().tokenName == "quote" ) {
    // cout << "Atom " ;
    
    return true ;
  } // if
  
  
  else if ( gTokens.back().typeNum == LEFTPAREN ) {
                                                
    if ( !GetToken() ) return false ;
    
    if ( SyntaxChecker() ) {
      if ( !GetToken() ) return false ;
      
      while ( SyntaxChecker() ) {
        if ( !GetToken() ) return false ;
      } // while
      
      if ( gErrorMsgType != NOT_S_EXP_ERROR ) return false;
      
      if ( gTokens.back().typeNum == DOT ) {
        // cout << "Dot " ;
        if ( GetToken() ) {
          if ( SyntaxChecker() ) {
            if ( !GetToken() ) return false ;
          } // if  syntax checker
          
          else {
            if ( gErrorMsgType == NOT_S_EXP_ERROR ) {
              SetErrorMsg( LEFT_ERROR, gTokens.back().tokenName,
                           gTokens.back().tokenLine, gTokens.back().tokenColumn  ) ;
            } // if
            
            return false;
            
          } // else
        } // if: GetToken()
        
        else return false ;
      } // if
      
      if ( gTokens.back().typeNum == RIGHTPAREN ) return true;
      
      else {
        SetErrorMsg( RIGHT_ERROR, gTokens.back().tokenName,
                     gTokens.back().tokenLine, gTokens.back().tokenColumn  ) ;
        return false;
      } // else
    } // if
    
    else {
      if ( gErrorMsgType == NOT_S_EXP_ERROR ) {
        SetErrorMsg( LEFT_ERROR, gTokens.back().tokenName,
                     gTokens.back().tokenLine, gTokens.back().tokenColumn  ) ;
      } // if
      
      return false ;
    } // else
    
  } // if
  
  else if ( gTokens.back().typeNum == QUOTE && gTokens.back().tokenName == "'" ) {
    // cout << "Quote " ;
    Token temp ;
    gTokens.pop_back() ;
    temp.tokenName = "(" ;
    temp.typeNum = LEFTPAREN ;
    gTokens.push_back( temp ) ;
    temp.tokenName = "quote" ;
    temp.typeNum = QUOTE ;
    gTokens.push_back( temp ) ;

    
    if ( GetToken() ) {
      if ( SyntaxChecker() ) {
        temp.tokenName = ")" ;
        temp.typeNum = RIGHTPAREN ;
        gTokens.push_back( temp ) ;
        return true ;
      } // if :push right paren
      else {
        if ( gErrorMsgType == NOT_S_EXP_ERROR ) 
          SetErrorMsg( LEFT_ERROR, gTokens.back().tokenName,
                       gTokens.back().tokenLine, gTokens.back().tokenColumn  ) ;
        return false ;
      } // else
    } // if
    
    else return false ;
  } // if
  
  SetErrorMsg( NOT_S_EXP_ERROR, gTokens.back().tokenName,
               gTokens.back().tokenLine, gTokens.back().tokenColumn ) ;
  return false ;
  
} // SyntaxChecker()

// ------------------Print Function--------------------- //

void PrintAtom( int i ) {
  if ( gTokens[i].typeNum == FLOAT ) {
    cout << fixed << setprecision( 3 )
         << round( atof( gTokens[i].tokenName.c_str() ) * 1000 ) / 1000 << endl ;
  } // if
  else cout << gTokens[i].tokenName << endl  ;
} // PrintAtom()

bool NeedPrint( int i, int printParenNum ) {
  if ( i > 0 ) {
    if ( gTokens[i].typeNum == LEFTPAREN && gTokens[i-1].typeNum == DOT )       // . ( case
      return false ;
    
    else if ( gTokens[i].typeNum == DOT ) {                                     // atom . Nil case
      if ( gTokens[i+1].typeNum == NIL ) return false ;
      if ( !IsAtom( gTokens[i+1].typeNum ) ) return false ;
    } // if
    
    else if ( gTokens[i].typeNum == NIL && gTokens[i-1].typeNum == DOT )         // atom . Nil case
      return false ;
    
    else if ( gTokens[i].typeNum == RIGHTPAREN ) {
      if ( printParenNum > 1 ) return true;
      else return false ;
    } // if

  } // if
  
  return true ;
  
} // NeedPrint()

void PrintSpace( int printParenNum ) {
  for ( int i = 0 ; i < printParenNum ; i++ )
    cout << "  " ;
} // PrintSpace()

void PrintSExp() {
  int printParenNum = 0 ;
  Token temp ;
  bool lineReturn = false ;
  
  for ( int i = 0 ; i < gTokens.size() ; i++ ) {
    if ( gTokens.size() == 1 ) PrintAtom( i ) ;

    else {
      if ( NeedPrint( i, printParenNum ) ) {
        
        if ( gTokens[i].typeNum == RIGHTPAREN ) printParenNum -- ;
        
        if ( lineReturn ) {
          PrintSpace( printParenNum ) ;
          lineReturn = false ;
        } // if
        
        if ( gTokens[i].typeNum == QUOTE ) {
          cout << gTokens[i].tokenName << endl ;
          lineReturn = true ;
        } // if
        
        else if ( gTokens[i].typeNum == LEFTPAREN ) {
          cout << gTokens[i].tokenName << " " ;
          printParenNum ++ ;
        } // if
        
        else if ( gTokens[i].typeNum == RIGHTPAREN ) {
          cout << gTokens[i].tokenName << endl ;
          lineReturn = true ;
        } // if
        
        else if ( gTokens[i].typeNum == DOT ) {
          cout << gTokens[i].tokenName << endl ;
          lineReturn = true ;
        } // if
        
        
        else if ( IsAtom( gTokens[i].typeNum ) ) {
          PrintAtom( i ) ;
          lineReturn = true ;
        } // if
        
      } // if
    } // else
    
    if ( i == ( gTokens.size()-1 ) && printParenNum > 0 ) {
      while ( printParenNum > 0 ) {
        printParenNum -- ;
        PrintSpace( printParenNum ) ;
        cout << ")" << endl;
      } // while
    } // if
  } // for
  
  cout << endl;
} // PrintSExp()


void PrintErrorMessage() {
  if ( gErrorMsgType == LEFT_ERROR ||  gErrorMsgType == NOT_S_EXP_ERROR )
    cout << "ERROR (unexpected token) : atom or '(' expected when token at Line "
    << gErrorLine << " Column " << gErrorColumn << " is >>" << gErrorMsgName << "<<" << endl << endl;
  else if ( gErrorMsgType == RIGHT_ERROR )
    cout << "ERROR (unexpected token) : ')' expected when token at Line "
    << gErrorLine << " Column " << gErrorColumn << " is >>" << gErrorMsgName << "<<" << endl << endl;
  else if ( gErrorMsgType == CLOSE_ERROR )
    cout << "ERROR (no closing quote) : END-OF-LINE encountered at Line "
    << gErrorLine << " Column " << gErrorColumn << endl << endl;
  else if ( gErrorMsgType == EOF_ERROR )
    cout << "ERROR (no more input) : END-OF-FILE encountered" ;
} // PrintErrorMessage()
  

// ------------------Main Function--------------------- //

int main() {
  cout << "Welcome to OurScheme!" << endl << endl ;
  char uTestNum = '\0' ;
  while ( uTestNum != '\n' )  { 
    uTestNum = cin.get();
  } // while
  
  
  do {
    cout << "> " ;
    if ( GetToken() ) {
      if ( SyntaxChecker() ) {
        if ( !ExitDetect() ) {
          PrintSExp() ;
          ClearSpaceAndOneLine() ;
        } // if
      } // if
      
      else {
        PrintErrorMessage() ;
        ClearInput() ;
        InitialLineColumn() ;
      } // else
    } // if
    
    else {
      PrintErrorMessage() ;
      ClearInput() ;
      InitialLineColumn() ;
    } // else
    
    GlobalVariableReset() ;
  } while ( !gIsEnd );

  
  cout << endl << "Thanks for using OurScheme!" << endl ;
  return 0 ;
  
} // main()
