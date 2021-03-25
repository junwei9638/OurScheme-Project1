#include <iostream>
#include <string>
#include<vector>

using namespace std;

struct  Token {
  string token = "\0" ;
  int typeNum = 0 ;
}; // TokenType


enum Type {
  Int, String, Dot, Float, Nil, T, Quote, Symbol, LeftParen, RightParen
}; // enum

enum Error {
  LeftError, RightError, CloseError, EofError
}; // enum

vector<struct Token> Token;

int gLine = 1 ;
int gColumn = 0 ;

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
      
    } // else
    
    atomType = Symbol ;
    if ( IsNum && !IsSymbol ) atomExp = NumProcess( atomExp );
    
    return  atomExp;
  } // AtomAnalyze
  
  string GetAtom( ){
    string atomExp = "\0" ;
    char ch = '\0' ;
    char peek = cin.peek() ;
    while( peek != '\n' && peek != EOF &&
          peek != ' ' && peek != ';') {
      ch = cin.get() ;
      gColumn ++ ;
      atomExp = atomExp + ch;
      peek = cin.peek() ;
    } // while
    
    atomExp = AtomAnalyze( atomExp );
    
    return atomExp ;
  } // GetAtom
  
  
  bool IsAtom(){
    if( Token.back().typeNum == Symbol || Token.back().typeNum == Int ||
       Token.back().typeNum == Float || Token.back().typeNum == String ||
       Token.back().typeNum == Nil || Token.back().typeNum == T  ||
       Token.back().typeNum == LeftParen || Token.back().typeNum == RightParen )
      return true ;
    
    else return false ;
  } // IsAtom()
  
  bool GetToken(){
    struct Token exp;
    char peek = cin.peek() ;
    
    if( peek == '\n' ) return false;
    
    while( peek == ' ' || peek == '\t' || peek == '\n' ) {
      cin.get() ;
      gColumn++ ;
      peek = cin.peek() ;
    } // while                                                  // white space case
    
    if( peek == ';' ) {
      while( peek != '\n' ){
        cin.get() ;
        gColumn ++ ;
        peek = cin.peek() ;
      } // while
    } // else                                                        // comment case
    
    else if( peek == '(' ) {
      exp.token = cin.get() ;
      gColumn ++ ;
      atomType = LeftParen;
    } // if
    
    else if( peek == ')' ) {
      exp.token = cin.get() ;
      gColumn ++ ;
      atomType = RightParen;
    } // if
    
    else if( peek == '"' ) exp.token = StringProcess( ) ;
    
    else exp.token = GetAtom();
    
    if( exp.token != "" ){
      exp.typeNum = atomType ;
      Token.push_back( exp ) ;
    } // if
    
    return true;
    
  } // GetToken()
  
  bool ReadSExp(){
    if( GetToken() ){
      if( IsAtom() ) return true ;
      else return false;
    } //if
    
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
  class1.ReadSExp();
  cout << gColumn << "\t" << gLine <<endl;
  
  cout << "\n" << "Thanks for using OurScheme!" << "\n" ;
  return 0;
  
} // main()
