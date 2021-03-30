# include <iostream>
# include <string>
# include <vector>

using namespace std;

struct  Token {
  string tokenName  ;
  int typeNum  ;
}; // TokenType


struct TokenTree {
  Token * leftToken  ;
  Token * rightToken ;
  TokenTree * leftNode  ;
  TokenTree * rightNode  ;
  TokenTree * backNode  ;
}; // TokenType


enum Type {
  INT, STRING, DOT, FLOAT, NIL, T, QUOTE, SYMBOL, LEFTPAREN, RIGHTPAREN
}; // Type

enum Error {
  LEFTERROR, RIGHTERROR, CLOSEERROR, EOFERROR, NOERROR
}; // Error

Token gErrorMsg ;
vector<Token> gTokens;
TokenTree * gTreeRoot = NULL;
TokenTree * gCurrentNode = NULL ;

int gLine = 1 ;
int gColumn = 0 ;
bool gIsEnd = false;
int gAtomType ;
  
string StringProcess() {
  string inputString = "\0" ;
  bool closeQuote = false ;
  char ch = cin.get() ;
  gColumn ++ ;
  inputString += ch ;
  char peek = cin.peek() ;
  gAtomType = STRING ;
  
  while ( closeQuote == false ) {
    
    ch = cin.get() ;
    gColumn ++ ;
    peek = cin.peek() ;
    
    if ( ch == '"' ) closeQuote = true ;
    
    if ( ch == '\\' && peek == '\\' ) {              // double slash
      cin.get();
      gColumn ++ ;
      inputString += '\\' ;
    } // if
    
    else if ( ch == '\\' && peek == 'n' ) {           // \n case
      cin.get() ;
      gColumn ++ ;
      inputString += '\n' ;
    } // if
    
    else if ( ch == '\\' && peek == 't' ) {          // \t case
      cin.get() ;
      gColumn ++ ;
      inputString += '\t' ;
    } // if
    
    else if ( ch == '\\' && peek == '"' ) {          // \" case
      cin.get() ;
      gColumn ++ ;
      inputString += '"' ;
    } // if
    
    else inputString += ch ;
    
  } // while
  
  return inputString ;
} // StringProcess()

string NumProcess( string atomExp ) {
  int floatLength = 0 ;
  int intLength = 0 ;
  int float4 = 0 ;
  if ( atomExp[0] == '+' ) atomExp = atomExp.substr( 1, atomExp.length() - 1 ) ;  // '+' case
  
  int dot = ( int ) atomExp.find( '.' );
  if ( dot != atomExp.npos ) {
    gAtomType = FLOAT ;
    floatLength = ( int ) ( atomExp.length() - 1 ) - dot ;
    intLength = ( int ) ( atomExp.length() - 1 ) - floatLength ;
    
    if ( atomExp[0] == '.'  ) atomExp = "0" + atomExp ;
    
    if ( floatLength > 3 ) {
      if ( ( int ) ( atomExp[intLength + 4] ) > 52 ) {
        float4 = ( ( int ) ( atomExp[intLength + 3] ) ) + 1 ;
        atomExp = atomExp.substr( 0, dot + 3 ) + ( char ) float4 ;
      } // if
      
      else atomExp = atomExp.substr( 0, dot + 4 ) ;                // 四捨五入
    } // if                                                        // float > 3
    
    else if ( floatLength < 3 ) {
      for ( int i = 0 ; i < ( 3 - floatLength ) ; i++ )
        atomExp = atomExp + "0" ;
    } // if                                                        // float < 3
  } // if                                                          // float case
  
  
  else gAtomType = INT ;
  return atomExp ;
} // NumProcess()

string AtomAnalyze( string atomExp ) {
  bool isNum = false ;
  bool isSymbol = false ;
  int signBit = 0 ;
  if ( atomExp == "#t" || atomExp == "t" ) {
    atomExp = "#t" ;
    gAtomType = T ;
  } // if
  
  else if ( atomExp == "nil" || atomExp == "#f" ) {
    atomExp = "nil" ;
    gAtomType = NIL ;
  } // if
  else if ( atomExp == "'"  ) {
    atomExp = "'" ;
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
        isNum = true;
      } // if
      
      else isSymbol = true;
    } // for                                                           // Num Judge
    
    if ( signBit > 1 ) isNum = false ;
    
    gAtomType = SYMBOL ;
    if ( isNum && !isSymbol ) atomExp = NumProcess( atomExp );
  } // else

  
  return  atomExp;
} // AtomAnalyze()

string GetAtom( ) {
  string atomExp = "\0" ;
  char ch = '\0' ;
  char peek = cin.peek() ;
  while ( peek != '\n' &&
          peek != ' ' && peek != ';' &&
          peek != '(' && peek != ')' && peek != '"' ) {
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
  while ( peek == ' ' || peek == '\t' || peek == '\n' || peek == ';' ) {
    if ( peek == ';' ) {
      while ( peek != '\n' ) {
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

  
  if ( peek == '\0' ) {
    cout << "in" ;
    gIsEnd = true ;
    return false;
  } // if
  
  else {
    
    if ( peek == '(' ) {
      token.tokenName = cin.get() ;
      gColumn ++ ;
      
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
      gAtomType = RIGHTPAREN;
    } // if             // right paren
    
    else if ( peek == '"' ) token.tokenName = StringProcess( ) ; // string
    
    else token.tokenName = GetAtom();              // symbol
    
    token.typeNum = gAtomType ;
    gTokens.push_back( token ) ;
    
    return true;
  } // else
  
} // GetToken()

bool IsAtom() {
  if ( gTokens.back().typeNum == SYMBOL || gTokens.back().typeNum == INT ||
       gTokens.back().typeNum == FLOAT || gTokens.back().typeNum == STRING ||
       gTokens.back().typeNum == NIL || gTokens.back().typeNum == T )
    return true ;
  
  else return false ;
} // IsAtom()

void InitialNode() {
  gCurrentNode->leftNode = NULL ;
  gCurrentNode->leftToken = NULL ;
  gCurrentNode->rightNode = NULL ;
  gCurrentNode->rightToken = NULL ;
} // InitialNode()

void InsertAtomToTree() {
  if ( gCurrentNode->leftToken == NULL && gCurrentNode->leftNode == NULL ) {
    gCurrentNode->leftToken = new Token;
    gCurrentNode->leftToken->tokenName = gTokens.back().tokenName ;
    gCurrentNode->leftToken->typeNum = gTokens.back().typeNum ;
  } // if
  
  else if ( gCurrentNode->rightToken == NULL && gTokens[ gTokens.size() - 2 ].typeNum == DOT ) {
    gCurrentNode->rightToken = new Token;
    gCurrentNode->rightToken->tokenName = gTokens.back().tokenName ;
    gCurrentNode->rightToken->typeNum = gTokens.back().typeNum ;
  } // if
  
  else if ( gCurrentNode->rightToken == NULL && gTokens[ gTokens.size() - 2 ].typeNum != DOT ) {
    gCurrentNode->rightNode = new TokenTree ;
    gCurrentNode->rightNode->backNode = gCurrentNode ;
    gCurrentNode = gCurrentNode->rightNode ;
    InitialNode() ;
    gCurrentNode->leftToken = new Token;
    gCurrentNode->leftToken->tokenName = gTokens.back().tokenName ;
    gCurrentNode->leftToken->typeNum = gTokens.back().typeNum ;
  } // if

} // InsertAtomToTree()

void BuildTree() {
  
  while( gCurrentNode->rightNode != NULL ) gCurrentNode = gCurrentNode->backNode ;
  
  if ( gCurrentNode->leftToken == NULL && gCurrentNode->leftNode == NULL ) {
    gCurrentNode->leftNode = new TokenTree ;
    gCurrentNode->leftNode->backNode = gCurrentNode ;
    gCurrentNode = gCurrentNode->leftNode ;
    InitialNode() ;
  } // if
  
  else if ( gCurrentNode->rightToken == NULL && gCurrentNode->rightNode == NULL ) {
    gCurrentNode->rightNode = new TokenTree ;
    gCurrentNode->rightNode->backNode = gCurrentNode ;
    gCurrentNode = gCurrentNode->rightNode ;
    InitialNode() ;
  } // else

} // BuildTree()

void SetErrorMsg( int errorType, string errorToken ) {
  gErrorMsg.typeNum = errorType ;
  gErrorMsg.tokenName = errorToken ;
} // SetErrorMsg()

bool SyntaxChecker() {
  if ( IsAtom() ) {
    // cout << "Atom " ;
    if ( gTreeRoot == NULL ) {
      gTreeRoot = new TokenTree ;
      gCurrentNode = gTreeRoot ;
      InitialNode() ;
    } // if
    
    InsertAtomToTree();
    return true ;
  } // if
  
  else if ( gTokens.back().typeNum == LEFTPAREN ) {
    // cout << "Left " ;
    if ( !GetToken() ) {
      cout << "error1" ;
      return false ;
    } // if
    
    if ( gTreeRoot == NULL ) {
      gTreeRoot = new TokenTree ;
      gCurrentNode = gTreeRoot ;
      InitialNode();
    } // if
    
    else BuildTree() ;                        // create node
    
    while ( SyntaxChecker() ) {
      if ( !GetToken() ) {
        cout << "error2" ;
        return false ;
      } // if
    } // while
    
    if ( gTokens.back().typeNum == DOT ) {
      // cout << "Dot " ;
      if ( GetToken() ) {
        if ( SyntaxChecker() ) {
          if ( !GetToken() ) {
            cout << "error3" ;
            return false ;
          } // if  no token

        } // if  syntax checker
        
        else {
          SetErrorMsg( LEFTERROR, gTokens.back().tokenName ) ;
          return false ;
        } // else
      } // if
      
      else {
        cout << "error5" ;
        return false ;
      } // else
    } // if
    
    if ( gTokens.back().typeNum == RIGHTPAREN ) {
      // cout << "Right " ;
      gCurrentNode = gCurrentNode->backNode;
      return true;
    } // if
    
  } // if
  
  
  else if ( gTokens.back().typeNum == QUOTE ) {
    // cout << "Quote " ;
    if ( GetToken() ) {
      if ( SyntaxChecker() ) {
        if ( !GetToken() ) {
          cout << "error6" ;
          return false ;
        } // if
      } // if
      
      else {
        cout << "error7" ;
        return false ;
      } // else
    } // if
    else {
      cout << "error8" ;
      return false ;
    } // else
  } // if
  
  SetErrorMsg( LEFTERROR, gTokens.back().tokenName ) ;
  return false;
  
} // SyntaxChecker()

void PrintSExp() {
  
  
} // PrintSExp()


void PrintErrorMessage( int errorType, string errorToken ) {
  if ( errorType == LEFTERROR )
    cout << "ERROR (unexpected token) : atom or '(' expected when token at Line "
    << gLine << " Column " << gColumn << " is >>" << errorToken << "<< " << endl;
  else if ( errorType == RIGHTERROR )
    cout << "ERROR (unexpected token) : ')' expected when token at Line "
    << gLine << " Column " << gColumn << " is >>" << errorToken << "<< " << endl;
  else if ( errorType == CLOSEERROR )
    cout << "ERROR (no closing quote) : END-OF-LINE encountered at Line "
    << gLine << " Column " << gColumn << endl;
  else if ( errorType == EOFERROR )
    cout << "ERROR (no more input) : END-OF-FILE encountered" << endl;
} // PrintErrorMessage()
  


int main() {
  cout << "Welcome to OurScheme!" << "\n" ;
  cout << "> " ;
  do {
    SetErrorMsg( NOERROR, "\0" ) ;
    
    GetToken() ;
    SyntaxChecker() ;
    if ( gErrorMsg.typeNum != NOERROR )
      PrintErrorMessage( gErrorMsg.typeNum, gErrorMsg.tokenName ) ;
    else
      PrintSExp() ;
    // for ( int i = 0 ; i < gTokens.size() ; i++ )
      // cout << i << ".  " << gTokens[i].typeNum << "\t" << gTokens[i].tokenName << endl ;
    
    gTreeRoot = NULL ;
    gTokens.clear() ;
    gColumn = 0 ;
    gLine = 0 ;
  } while ( !gIsEnd );

  cout << endl << gColumn << "\t" << gLine << endl;
  
  cout << "\n" << "Thanks for using OurScheme!" << "\n" ;
  return 0;
  
} // main()
