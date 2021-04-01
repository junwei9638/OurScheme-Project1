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

vector<Token> gTokens;
TokenTree * gTreeRoot = NULL;
TokenTree * gCurrentNode = NULL ;

int gLine = 1 ;
int gColumn = 0 ;

bool gIsEnd = false;
int gAtomType ;

string gErrorMsgName = "\0" ;
int gErrorMsgType = NOERROR ;
int gErrorLine = 0 ;
int gErrorColumn = 0 ;

// ------------------Setting Function--------------------- //

void ClearInput() {
  while ( cin.get() != '\n' ) cin.get() ;
} // ClearInput()

void SetErrorMsg( int errorType, string errorToken ) {
  gErrorMsgType = errorType ;
  gErrorMsgName = errorToken ;
  gErrorLine = gLine ;
  gErrorColumn = gColumn ;
} // SetErrorMsg()

void GlobalVariableReset() {
  gTreeRoot = NULL ;
  gTokens.clear() ;
  gColumn = 0 ;
  gLine = 1 ;
  gErrorLine = 0;
  gErrorColumn = 0;
} // GlobalVariableReset()

bool ExitDetect() {
  int leftparen = -1 ;
  int exit = -1 ;
  int rightparen = -1 ;
  
  for ( int i = 0 ; i < gTokens.size() ; i++ ) {
    if ( gTokens[i].tokenName == "(" ) leftparen = i ;
    if ( gTokens[i].tokenName == "exit" ) exit = i ;
    if ( gTokens[i].tokenName == ")" ) rightparen = i ;
  } // for
  
  if ( exit > -1 && leftparen > -1 && rightparen > -1 && exit > leftparen && rightparen > exit ) {
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
  
  while ( closeQuote == false && peek != '\n' ) {
    
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
  
  if ( closeQuote == false ) {
    cin.get() ;
    gColumn ++ ;
    SetErrorMsg( CLOSEERROR, "\"" ) ;
    return "\0" ;
  } // if                                        // no closing quote
  
  return inputString ;
} // StringProcess()

string NumProcess( string atomExp ) {
  int floatLength = 0 ;
  int intLength = 0 ;
  int float4 = 0 ;
  if ( atomExp[0] == '+' ) atomExp = atomExp.substr( 1, atomExp.length() - 1 ) ;  // '+' case
  if ( atomExp[0] == '.'  ) atomExp = "0" + atomExp ;                             // .xxx case
  if ( atomExp[0] == '-' && atomExp[1] == '.' ) atomExp.insert( 1, "0" ) ;        // -.xxx case
  
  int dot = ( int ) atomExp.find( '.' );
  if ( dot != atomExp.npos ) {
    gAtomType = FLOAT ;
    floatLength = ( int ) ( atomExp.length() - 1 ) - dot ;
    intLength = ( int ) ( atomExp.length() - 1 ) - floatLength ;
    
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
  int digitBit = 0 ;
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
        if ( ( int ) atomExp[i] >= 48 && ( int ) atomExp[i] <= 57 ) digitBit ++ ;
        isNum = true;
      } // if
      
      else isSymbol = true;
    } // for                                                           // Num Judge
    
    if ( signBit > 1 || digitBit == 0 ) isNum = false ;
    
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
    
    else if ( peek == '"' ) {
      token.tokenName = StringProcess() ;
      if ( token.tokenName == "\0" ) return false ;
    } // if                                        // string
    
    else token.tokenName = GetAtom();              // symbol
    
    token.typeNum = gAtomType ;
    gTokens.push_back( token ) ;
    
    return true;
  } // else
  
} // GetToken()

// ------------------Tree Build--------------------- //

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
  if ( gTreeRoot != NULL ) {
    if ( gCurrentNode->leftToken == NULL ) {                              // left token null
      if ( gCurrentNode->leftNode == NULL ) {                             // left node null -> insert left
        gCurrentNode->leftToken = new Token ;
        gCurrentNode->leftToken->tokenName = gTokens.back().tokenName ;
        gCurrentNode->leftToken->typeNum = gTokens.back().typeNum ;
      } // if
      
      else if ( gCurrentNode->leftNode != NULL ) {                        // left node !null
        while ( gCurrentNode->rightNode != NULL )                         // find right node null
          gCurrentNode = gCurrentNode->backNode ;                         // and insert right token
        
        gCurrentNode->rightToken = new Token ;
        gCurrentNode->rightToken->tokenName = gTokens.back().tokenName ;
        gCurrentNode->rightToken->typeNum = gTokens.back().typeNum ;
      } // if
    } // if

    else if ( gCurrentNode->leftToken != NULL ) {                       // left token !null
      while ( gCurrentNode->rightToken != NULL )                        // find right node null
        gCurrentNode = gCurrentNode->backNode ;
      
      if ( gTokens[gTokens.size()-2].typeNum != DOT ) {                 // if !dot-> create right node
        gCurrentNode->rightNode = new TokenTree ;                       // and insert to left token
        gCurrentNode->rightNode->backNode = gCurrentNode;
        gCurrentNode = gCurrentNode->rightNode ;
        InitialNode();
        gCurrentNode->leftToken = new Token ;
        gCurrentNode->leftToken->tokenName = gTokens.back().tokenName ;
        gCurrentNode->leftToken->typeNum = gTokens.back().typeNum ;
      } // if
      
      else if ( gTokens[gTokens.size()-2].typeNum == DOT ) {            // if == dot-> insert right token
        gCurrentNode->rightToken = new Token ;
        gCurrentNode->rightToken->tokenName = gTokens.back().tokenName ;
        gCurrentNode->rightToken->typeNum = gTokens.back().typeNum ;
      } // if
    } // if
  } // if
} // InsertAtomToTree()

void BuildTree() {
  if ( gTreeRoot == NULL ) {
    gTreeRoot = new TokenTree ;
    gCurrentNode = gTreeRoot ;
    InitialNode();
  } // if
  
  else {
    if ( gCurrentNode->leftToken == NULL ) {                // left token null
      if ( gCurrentNode->leftNode == NULL ) {               // left node null-> create node
        gCurrentNode->leftNode = new TokenTree ;
        gCurrentNode->leftNode->backNode = gCurrentNode;
        gCurrentNode = gCurrentNode->leftNode ;
        InitialNode();
      } // if
      
      else if ( gCurrentNode->leftNode != NULL ) {          // left node !null
        while ( gCurrentNode->rightNode != NULL )           // find right node null-> create node
          gCurrentNode = gCurrentNode->backNode ;
        
        gCurrentNode->rightNode = new TokenTree ;
        gCurrentNode->rightNode->backNode = gCurrentNode;
        gCurrentNode = gCurrentNode->rightNode ;
        InitialNode();
      } // if
    } // if
    
    else if ( gCurrentNode->leftToken != NULL ) {         // left token !null
      while ( gCurrentNode->rightNode != NULL )           // find right node null-> create node
        gCurrentNode = gCurrentNode->backNode ;
      
      gCurrentNode->rightNode = new TokenTree ;
      gCurrentNode->rightNode->backNode = gCurrentNode;
      gCurrentNode = gCurrentNode->rightNode ;
      InitialNode();
    } // if
  } // else
  

} // BuildTree()


bool SyntaxChecker() {
  if ( IsAtom() ) {
    // cout << "Atom " ;
    
    if ( gTreeRoot != NULL ) InsertAtomToTree();
    return true ;
  } // if
  
  else if ( gTokens.back().typeNum == LEFTPAREN ) {
    // cout << "Left " ;
    if ( !GetToken() ) {
      SetErrorMsg( CLOSEERROR, gTokens.back().tokenName ) ;
      return false ;
    } // if
    
    BuildTree() ;                        // create node
    
    while ( SyntaxChecker() ) {
      if ( !GetToken() ) return false ;
    } // while
    
    
    if ( gTokens.back().typeNum == DOT ) {
      // cout << "Dot " ;
      if ( GetToken() ) {
        if ( SyntaxChecker() ) {
          if ( !GetToken() ) {
            SetErrorMsg( EOFERROR, gTokens.back().tokenName ) ;
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
      gCurrentNode = gCurrentNode->backNode ;
      return true;
    } // if
    
  } // if
  
  else if ( gTokens.back().typeNum == QUOTE ) {
    // cout << "Quote " ;
    if ( GetToken() ) {
      if ( SyntaxChecker() ) return true ;
      else {
        cout << "error7" ;
        return false ;
      } // else
    } // if
    
    else {
      SetErrorMsg( LEFTERROR, gTokens.back().tokenName ) ;
      return false ;
    } // else
  } // if
  
  SetErrorMsg( LEFTERROR, gTokens.back().tokenName ) ;
  return false;
  
} // SyntaxChecker()

// ------------------Print Function--------------------- //

void PrintSExp() {
  
  
} // PrintSExp()


void PrintErrorMessage() {
  if ( gErrorMsgType == LEFTERROR )
    cout << "ERROR (unexpected token) : atom or '(' expected when token at Line "
    << gErrorLine << " Column " << gErrorColumn << " is >>" << gErrorMsgName << "<< " << endl;
  else if ( gErrorMsgType == RIGHTERROR )
    cout << "ERROR (unexpected token) : ')' expected when token at Line "
    << gErrorLine << " Column " << gErrorColumn << " is >>" << gErrorMsgName << "<< " << endl;
  else if ( gErrorMsgType == CLOSEERROR )
    cout << "ERROR (no closing quote) : END-OF-LINE encountered at Line "
    << gErrorLine << " Column " << gErrorColumn << endl;
  else if ( gErrorMsgType == EOFERROR )
    cout << "ERROR (no more input) : END-OF-FILE encountered" << endl;
} // PrintErrorMessage()
  

// ------------------Main Function--------------------- //

int main() {
  cout << "Welcome to OurScheme!" << endl << endl ;
  int uTestNum = 0 ;
  // cin >> uTestNum ;
  do {
    cout << "> " ;
    if ( GetToken() ) {
      if ( SyntaxChecker() ) PrintSExp() ;
      else {
        PrintErrorMessage() ;
        ClearInput() ;
      } // else
    } // if
    
    else {
      PrintErrorMessage() ;
      ClearInput() ;
    } // else
    
    // if ( !ExitDetect() ) cout << gTokens.back().tokenName << endl << endl;
    
      
    for ( int i = 0 ; i < gTokens.size() ; i++ ) {
      cout << i << ".  " << gTokens[i].typeNum << "\t" << gTokens[i].tokenName << endl  ;
      // cout << "(" << gColumn << " , " << gLine << ")" << endl;
    } // for
      
    
    // cout << "(" << gColumn << " , " << gLine << ")" << endl;
    
    GlobalVariableReset() ;
  } while ( !gIsEnd );

  
  cout << "\n" << "Thanks for using OurScheme!" << "\n" ;
  return 0;
  
} // main()
