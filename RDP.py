# Michael Balas
# This program can be run as python2 or as python3
from __future__ import print_function

# A recursive descent parser in Python for simple arithemic expressions.
# The scanner is not separated from the parser. Each nonterminal corresponds
# to a function, the correspoding productions are shown as a comment at the
# header of the function. The grammar is modified to be predictive.

# tokens:
# NUM is an unsigned decimal number with at most 4 digits after . , at most
#     9 significant digits and no leading zeroes
# VAR is a 1 character identifier of upper case letters A to Z
# = - + * / ( ) ;
# define tokens
EOI = 0
NUM = 1
VAR = 2
EQ = 3
MINUS = 4
PLUS = 5
MULT = 6
DIVI = 7
LP = 8
RP = 9
SEMI = 10
ERR = 11

# Grammar
# prog -> VAR tail1 | NUM tail2 | MINUS {VAR NUM} tail2 | ( expr )
# tail1 -> = expr ; prog | {+ -} term tail2 | {* /} factor tail3 | epsilon
# expr -> term tail2
# tail2 -> {+ -} term tail2 | epsilon
# term -> factor tail3
# tail3 -> {* /} factor tail3 | epsilon
# factor -> ( expr ) | - {NUM VAR} | NUM | VAR

import sys
debug = True

def show(indent,name,s,spp):
    if debug:
        print(indent+name+'("',end='');
        j = len(s)
        for i in range(spp,j):
            print(s[i],sep="",end="")
        print('")',end='\n')
        return
    else:
        return
#end show

def x(indent,name1,name2):
    if debug:
        print(indent+"returned to "+name1+" from "+name2)
    else:
        return
#end x



def EatWhiteSpace(s,spp):
    j = len(s)
    if spp >= j:
        return spp

    while s[spp] == ' ' or s[spp] == '\n':
        spp=spp+1
        if spp >= j:
            break
        
    return spp
#end EatWhiteSpace


# prog -> VAR tail1 | NUM tail2 | MINUS {VAR NUM} tail2 | ( expr ) | epsilon
# function prog ------------------------------------------------------------
def prog(s,spp,indent):
    show(indent,'prog',s,spp)
    indent1 = indent+' '

    token = LookAhead(s,spp)
    if token == VAR:
        spp = ConsumeToken(s,spp)
        res,spp = tail1(s,spp,indent1)
        x(indent1,"prog","tail1")
        return res,spp
    elif token == NUM:
        spp = ConsumeToken(s,spp)
        res,spp = tail2(s,spp,indent1)
        x(indent1,"prog","tail2")
        return res,spp
    elif token == MINUS:
        spp = ConsumeToken(s,spp)
        token,spp = NextToken(s,spp)
        if token == NUM or token == VAR:
            res,spp = tail2(s,spp,indent1)
            x(indent1,"prog","tail2")
            return res,spp
        else:
            return False,spp
    elif token == LP:
        spp = ConsumeToken(s,spp)
        res,spp = expr(s,spp,indent1)
        x(indent1,"prog","expr")
        if not res:
            return False,spp
        token,spp = NextToken(s,spp)
        if token == RP:
            token = LookAhead(s,spp)
            if token == EOI:
                return True,spp
            else:
                res,spp = tail1(s,spp,indent1)
                x(indent1,"prog","tail1")
                return res,spp
        return (token == RP),spp
    else:
        return True,spp
#end prog



# tail1 -> = expr ; prog | {+ -} term tail2 | {* /} factor tail3 | epsilon
# function tail1 --------------------------------------------------------
def tail1(s,spp,indent):
    show(indent,'tail1',s,spp)
    indent1 = indent+' '

    token = LookAhead(s,spp)
    if token == EQ:
        spp = ConsumeToken(s,spp)
        res,spp = expr(s,spp,indent1)
        x(indent1,"tail1","expr")
        if not res:
            return False,spp
        token,spp = NextToken(s,spp)
        if token != SEMI:
            return False,spp
        res,spp = prog(s,spp,indent1)
        x(indent1,"tail1","prog")
        return res,spp
    elif token == PLUS or token == MINUS:
        spp = ConsumeToken(s,spp)
        res,spp = term(s,spp,indent1)
        x(indent1,"tail1","term")
        if not res:
            return False,spp
        res,spp = tail2(s,spp,indent1)
        x(indent1,"tail1","tail2")
        return res,spp
    elif token == MULT or token == DIVI:
        spp = ConsumeToken(s,spp)
        res,spp = factor(s,spp,indent1)
        x(indent1,"tail1","factor")
        if not res:
            return False,spp
        res,spp = tail3(s,spp,indent1)
        x(indent1,"tail1","tail3")
        return res,spp
    else:
        return True,spp  #epsilon
#end tail1

    

# expr -> term tail2
# function expr --------------------------------------------- 
def expr(s,spp,indent):
    show(indent,'expr',s,spp)
    indent1 = indent+' '

    res,spp = term(s,spp,indent1)
    x(indent1,"expr","term")
    if not res:
        return False,spp
    res,spp = tail2(s,spp,indent1)
    x(indent1,"expr","tail2")
    return res,spp
#end expr



# tail2 -> {+ -} term tail2 | epsilon
# function tail2 --------------------------------------------- 
def tail2(s,spp,indent):
    show(indent,'tail2',s,spp)
    indent1 = indent+' '

    token = LookAhead(s,spp)
    if token == PLUS or token == MINUS:
        spp = ConsumeToken(s,spp)
        res,spp = term(s,spp,indent1)
        x(indent1,"tail2","term")
        if not res:
            return False,spp
        res,spp = tail2(s,spp,indent1)
        x(indent1,"tail2","tail2")
        return res,spp
    else:
        return True,spp  #epsilon
#end tail2



# term -> factor tail3
# function term --------------------------------------------- 
def term(s,spp,indent):
    show(indent,'term',s,spp)
    indent1 = indent+' '

    res,spp = factor(s,spp,indent1)
    x(indent1,"term","factor")
    if not res:
        return False,spp
    res,spp = tail3(s,spp,indent1)
    x(indent1,"term","tail3")
    return res,spp
#end term



# tail3 -> {* /} factor tail3 | epsilon
# function tail3 --------------------------------------------- 
def tail3(s,spp,indent):
    show(indent,'tail3',s,spp)
    indent1 = indent+' '

    token = LookAhead(s,spp)
    if token == MULT or token == DIVI:
        spp = ConsumeToken(s,spp)
        res,spp = factor(s,spp,indent1)
        x(indent1,"tail3","factor")
        return res,spp
    else:
        return True,spp  #epsilon
#end tail3




# factor -> ( expr ) | - {NUM VAR} | NUM | VAR
# function factor --------------------------------------------- 
def factor(s,spp,indent):
    show(indent,'factor',s,spp)
    indent1 = indent+' '

    token,spp = NextToken(s,spp)
    if token == LP:
        res,spp = expr(s,spp,indent1)
        x(indent1,"factor","expr")
        if not res:
            return False,spp
        token,spp = NextToken(s,spp);
        return (token == RP),spp
    elif token == MINUS:
        token,spp = NextToken(s,spp)
        return (token == NUM or token == VAR),spp
    elif token == NUM or token == VAR:
        return True,spp
    else:
        return False,spp
#end factor


# the scanner ####################################################

# function LookAhead ------------------------------------------- 
def LookAhead(s,spp):
    j = len(s);
    i = spp
    if i >= j:
        return EOI
    while s[i]==" " or s[i]=="\n":
        i = i+1
        if i >= j:
            return EOI

    if s[i] == '=':
        return EQ
    elif s[i] == "-":
        return MINUS
    elif s[i] == "+":
        return PLUS
    elif s[i] == "*":
        return MULT
    elif s[i] == "/":
        return DIVI
    elif s[i] == "(":
        return LP
    elif s[i] == ")":
        return RP
    elif s[i] == ";":
        return SEMI
    elif (ord(s[i]) >= ord('0') and ord(s[i]) <= ord('9')):
        return NUM
    elif (ord(s[i]) >= ord('A') and ord(s[i]) <= ord('Z')):
        return VAR
    else:
        return ERR
#end LookAhead



# function NextToken --------------------------------------------- 
def NextToken(s,spp):
    spp1 = spp
    spp = EatWhiteSpace(s,spp)
    j = len(s)
    if spp >= j:
        return ERR,spp1

    if spp >= j:
        return EOI,spp
    elif s[spp] == '=':
        return EQ,spp+1
    elif s[spp] == "-":
        return MINUS,spp+1
    elif s[spp] == "+":
        return PLUS,spp+1
    elif s[spp] == "*":
        return MULT,spp+1
    elif s[spp] == "/":
        return DIVI,spp+1
    elif s[spp] == "(":
        return LP,spp+1
    elif s[spp] == ")":
        return RP,spp+1
    elif s[spp] == ";":
        return SEMI,spp+1
    elif (ord(s[spp])>=ord('0') and ord(s[spp])<=ord('9')):
        res,spp = Lnum(s,spp)
        if not res:
            return ERR,spp1
        if spp >= j:
            return True,spp
        if s[spp] != '.':
            return NUM,spp
        spp = spp+1  # eat .
        if spp >= j:
            return ERR,spp1
        res,spp = Rnum(s,spp);
        if not res:
            return ERR,spp1
        return NUM,spp
    elif (ord(s[spp])>=ord('A') and ord(s[spp])<=ord('Z')):
        return VAR,spp+1
    else:
        return ERR,spp1
#end NUM

def ConsumeToken(s,spp):
    token,spp = NextToken(s,spp)
    return spp
#end ConsumeToken


# function Rnum -------------------------------------------- 
def Rnum(s,spp):
    spp1 = spp # save position
    j = len(s)

    found = False
    i = 0
    while i<4 :
        if spp < j and ord(s[spp]) >= ord('0') and ord(s[spp]) <= ord('9'):
            found = True
            spp = spp+1
            if spp >= j:
                break
            i = i+1
        else:
            break
    #endwhile

    if found:
        return True,spp
    else:
        return False,spp1
#end Rnum


# function Lnum -------------------------------------------- 
def Lnum(s,spp):
    spp1 = spp # save position
    j = len(s)
    if spp >= j:
        return False,spp1

    if s[spp] == '0':
        spp = spp+1  # eat 0
        return True,spp

    i = 0
    found = False
    while i<4 :
        if spp < j and ord(s[spp]) >= ord('0') and ord(s[spp]) <= ord('9'):
            found = True
            spp = spp+1
            i = i+1
        else:
            break
    #endwhile

    if found:
        return True,spp
    else: 
        return False,spp1
#end Lnum


#main section
s = "(5+2)*3"
res,spp = prog(s,0,"");

# is there a leftover ?
if spp < len(s)-1:
    print("parse Error")
else:
    if res:
        print("parsing OK")
    else:
        print("parse Error")
#end main section
                



