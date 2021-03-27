#include <iostream>
#include <string>
#include<vector>

using namespace std;

struct  Token {
  string tokenName = "\0" ;
  int typeNum = 0 ;
}; // TokenType


struct TokenTree {
  Token * leftToken = NULL ;
  Token * rightToken = NULL;
  TokenTree * leftNode = NULL ;
  TokenTree * rightNode = NULL ;
}; // TokenType


enum Type {
  Int, String, Dot, Float, Nil, T, Quote, Symbol, LeftParen, RightParen
}; // Type

enum Error {
  LeftError, RightError, CloseError, EofError
}; // Error

vector<struct Token> Tokens;
TokenTree * treeRoot = NULL;
TokenTree * CurrentNode = NULL;

int gLine = 1 ;
int gColumn = 0 ;
bool gIsEnd = false;

class Project1 {
public:
  int atomType = 0 ;
  
  string StringProcess() {
    string inputString = "\0" ;
    bool closeQuote = false ;
    char ch = cin.get() ;
    gColumn ++ ;
    inputString += ch ;
    char peek = cin.peek() ;
    atomType = String ;
    
    while( closeQuote == false ){
      
      ch = cin.get() ;
      gColumn ++ ;
      peek = cin.peek() ;
      
      if( ch == '"' ) closeQuote = true ;
      
      if( ch == '\\' && peek == '\\' ) {              // double slash
        inputString += '\\' ;
        cin.get();
        gColumn ++ ;
        continue ;
      } // if
      
      else if( ch == '\\' && peek == 'n') {           // \n case
        cin.get() ;
        gColumn ++ ;
        inputString += '\n' ;
      } // if
      
      else if( ch == '\\' && peek == 't' ) {          // \t case
        cin.get() ;
        gColumn ++ ;
        inputString += '\t' ;
      } // if
      
      else if( ch == '\\' && peek == '"' ) {          // \" case
        cin.get() ;
        gColumn ++ ;
        inputString += '"' ;
      } // if
      
      else inputString += ch ;
      
    } // while
    
    return inputString ;
  } // StringProcess()
  
  string NumProcess( string atomExp ){
    int floatLength = 0 ;
    int intLength = 0 ;
    int float4 = 0 ;
    if( atomExp[0] == '+' ) atomExp = atomExp.substr( 1, atomExp.length() - 1 ) ;  // '+' case
    
    int dot = int( atomExp.find( '.' ) ) ;
    if( dot != atomExp.npos ) {
      atomType = Float ;
      floatLength = int( atomExp.length() - 1 ) - dot ;
      intLength = int( atomExp.length() - 1 ) - floatLength ;
      
      if( floatLength > 3 ) {
        if( int( atomExp[intLength + 4] ) > 52 ){
          float4 = ( int( atomExp[intLength + 3] ) ) + 1 ;
          atomExp = atomExp.substr( 0, dot + 3 ) + char( float4 );
        } // if
        
        else atomExp = atomExp.substr( 0, dot + 4 ) ;                // 四捨五入
      } // if                                                        // float > 3
      
      else if ( floatLength < 3 ) {
        for( int i = 0; i < ( 3 - floatLength ) ; i++ ) atomExp = atomExp + "0" ;
      } // if                                                        // float < 3
    } // if                                                          // float case
    
    
    else atomType = Int ;
    return atomExp ;
  } // NumProcess()
  
  string AtomAnalyze( string atomExp ) {
    bool IsNum = false ;
    bool IsSymbol = false ;
    int signBit = 0 ;
    if ( atomExp == "#t" || atomExp == "t" ){
      atomExp = "#t" ;
      atomType = T ;
    } // if
    
    else if ( atomExp == "nil" || atomExp == "#f" ){
      atomExp = "nil" ;
      atomType = Nil ;
    } // if
    else if ( atomExp == "'"  ){
      atomExp = "'" ;
      atomType = Quote ;
    } // if
    else if ( atomExp == "." ){
      atomExp = "." ;
      atomType = Dot ;
    } // if
    
    else{
      
      for( int i = 0; i < atomExp.length() ; i ++ ) {
        if( ( ( int(atomExp[i]) >= 48 && int(atomExp[i]) <= 57 ) ||
             atomExp[i] == '+' || atomExp[i] == '-' ||  atomExp[i] == '.' ) ) {
          if( atomExp[i] == '+' || atomExp[i] == '-' )signBit++ ;
          IsNum = true;
        } // if
        
        else IsSymbol = true;
      } // for                                                           // Num Judge
      
      if( signBit > 1 ) IsNum = false ;
      
      atomType = Symbol ;
      if ( IsNum && !IsSymbol ) atomExp = NumProcess( atomExp );
    } // else

    
    return  atomExp;
  } // AtomAnalyze()
  
  string GetAtom( ){
    string atomExp = "\0" ;
    char ch = '\0' ;
    char peek = cin.peek() ;
    while( peek != '\n' && peek != EOF &&
          peek != ' ' && peek != ';'&&
          peek != '(' && peek != ')' && peek != '"') {
      ch = cin.get() ;
      gColumn ++ ;
      atomExp = atomExp + ch;
      peek = cin.peek() ;
    } // while
    
    atomExp = AtomAnalyze( atomExp );
    
    return atomExp ;
  } // GetAtom()
  
  char GetChar( ){
    char peek = '\0' ;
    peek = cin.peek() ;
    while( peek == ' ' || peek == '\t' || peek == '\n' ) {
      cin.get() ;
      gColumn ++ ;
      peek = cin.peek() ;
      while( peek == ';' ) {
        while( peek != '\n' ){
          cin.get() ;
          gColumn ++ ;
          peek = cin.peek() ;
        } // while
      } // while
    } // while
    
    return peek ;
  } // GetChar()
  
  bool GetToken(){
    struct Token token;
    char peek = GetChar() ;
    
    if( peek == '\0' ) {
      gIsEnd = true ;
      return false;
    } // if
    
    else{
      
      if( peek == '(' ) {
        token.tokenName = cin.get() ;
        gColumn ++ ;
        
        peek = GetChar() ;
        
        if( peek == ')'){
          token.tokenName = "nil" ;
          cin.get() ;
          gColumn ++ ;
          atomType = Nil ;
        } // if                                                   // () case
        
        else
          atomType = LeftParen;
      } // if                                         // left praen
      
      else if( peek == ')' ) {
        token.tokenName = cin.get() ;
        gColumn ++ ;
        atomType = RightParen;
      } // if             // right paren
      
      else if( peek == '"' ) token.tokenName = StringProcess( ) ; // string
      
      else token.tokenName = GetAtom();              // symbol
      
      token.typeNum = atomType ;
      Tokens.push_back( token ) ;
      
      return true;
    } // else
    
  } // GetToken()
  
  bool IsAtom(){
    if( Tokens.back().typeNum == Symbol || Tokens.back().typeNum == Int ||
       Tokens.back().typeNum == Float || Tokens.back().typeNum == String ||
       Tokens.back().typeNum == Nil || Tokens.back().typeNum == T )
      return true ;
    
    else return false ;
  } // IsAtom()
  
  void InsertAtomToTree(){
    if( CurrentNode->leftToken == NULL ){
      CurrentNode->leftToken = new Token;
      CurrentNode->leftToken->tokenName = Tokens.back().tokenName ;
      CurrentNode->leftToken->typeNum = Tokens.back().typeNum ;
    } // if
    else {
      CurrentNode->rightToken = new Token;
      CurrentNode->rightToken->tokenName = Tokens.back().tokenName ;
      CurrentNode->rightToken->typeNum = Tokens.back().typeNum ;
    } // else
  } // InsertAtomToTree
  
  void BuildTree() {
    if( CurrentNode->leftToken == NULL ) {
      CurrentNode->leftNode = new TokenTree ;
      CurrentNode = CurrentNode->leftNode ;
    } // if
    
    else {
      CurrentNode->rightNode = new TokenTree ;
      CurrentNode = CurrentNode->rightNode ;
    } // else
  } // BuildTree
  
  bool SyntaxChecker(){
    if( IsAtom() ) {
      cout << "Atom " ;
      InsertAtomToTree();
      return true ;
    } // if
    
    else if( Tokens.back().typeNum == LeftParen ){
      cout << "Left " ;
      if( !GetToken() ) {
        cout<< "error1" ;
        return false ;
      } // if
      
      if( treeRoot == NULL ){
        treeRoot = new TokenTree ;
        CurrentNode = treeRoot ;
      } // if
      
      else BuildTree() ;                        // create node
      

    
      while( SyntaxChecker() ) {
        if( !GetToken() ) {
          cout<< "error2" ;
          return false ;
        } // if
      } // while
      
      if( Tokens.back().typeNum == Dot ){
        cout << "Dot " ;
        if( GetToken() ) {
          if( SyntaxChecker() ) {
            if( !GetToken() ) {
              cout<< "error3" ;
              return false ;
            } // if

          } // if
          
          else {
            cout<< "error4" ;
            return false ;
          } // else
        } // if
        
        else {
          cout<< "error5" ;
          return false ;
        } // else
      } // if
      
      if( Tokens.back().typeNum == RightParen ){
        cout << "Right " ;
        return true;
      } // if
      
    } // if
    
    
    else if( Tokens.back().typeNum == Quote ){
      cout << "Quote " ;
      if( GetToken() ) {
        if( SyntaxChecker() ) {
          if( !GetToken() ) {
            cout<< "error6" ;
            return false ;
          } // if
        } // if
        
        else {
          cout<< "error7" ;
          return false ;
        } // else
      } // if
      else {
        cout<< "error8" ;
        return false ;
      } // else
    } // if
      
    return false;
    
  } // ReadSExp()
  
  void PrintSExp(){
  } // PrintSExp()
  
  void PrintErrorMessage( int errorType, string errorToken ) {
    if( errorType == LeftError )
      cout << "ERROR (unexpected token) : atom or '(' expected when token at Line "
      << gLine << " Column " << gColumn << " is >>" << errorToken << "<< " << endl;
    else if( errorType == RightError )
      cout << "ERROR (unexpected token) : ')' expected when token at Line "
      << gLine << " Column " << gColumn << " is >>" << errorToken << "<< " << endl;
    else if( errorType == CloseError )
      cout << "ERROR (no closing quote) : END-OF-LINE encountered at Line "
      << gLine << " Column " << gColumn << endl;
    else if( errorType == EofError )
      cout << "ERROR (no more input) : END-OF-FILE encountered" << endl;
  } // PrintErrorMessage()
  
}; // Class project1


int main() {
  cout << "Welcome to OurScheme!" <<"\n" ;
  Project1 class1;
  cout << "> " ;
  do {
    class1.GetToken();
    cout << endl ;
    class1.SyntaxChecker();
    CurrentNode = treeRoot ;
    for( int i = 0; i<Tokens.size(); i++) cout << endl << Tokens[i].typeNum <<"\t"<< Tokens[i].tokenName;
  } while ( !gIsEnd );

  cout << endl << gColumn << "\t" << gLine <<endl;
  
  cout << "\n" << "Thanks for using OurScheme!" << "\n" ;
  return 0;
  
} // main()
