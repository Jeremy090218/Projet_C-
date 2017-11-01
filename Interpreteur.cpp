#include "Interpreteur.h"
#include <stdlib.h>
#include <iostream>
using namespace std;

Interpreteur::Interpreteur(ifstream & fichier) :
m_lecteur(fichier), m_table(), m_arbre(nullptr) {
}

void Interpreteur::analyse() {
  m_arbre = programme(); // on lance l'analyse de la première règle
}

void Interpreteur::tester(const string & symboleAttendu) const throw (SyntaxeException) {
  // Teste si le symbole courant est égal au symboleAttendu... Si non, lève une exception
  static char messageWhat[256];
  if (m_lecteur.getSymbole() != symboleAttendu) {
    sprintf(messageWhat,
            "Ligne %d, Colonne %d - Erreur de syntaxe - Symbole attendu : %s - Symbole trouvé : %s",
            m_lecteur.getLigne(), m_lecteur.getColonne(),
            symboleAttendu.c_str(), m_lecteur.getSymbole().getChaine().c_str());
    throw SyntaxeException(messageWhat);
  }
}

void Interpreteur::testerEtAvancer(const string & symboleAttendu) throw (SyntaxeException) {
  // Teste si le symbole courant est égal au symboleAttendu... Si oui, avance, Sinon, lève une exception
  tester(symboleAttendu);
  m_lecteur.avancer();
}

void Interpreteur::erreur(const string & message) const throw (SyntaxeException) {
  // Lève une exception contenant le message et le symbole courant trouvé
  // Utilisé lorsqu'il y a plusieurs symboles attendus possibles...
  static char messageWhat[256];
  sprintf(messageWhat,
          "Ligne %d, Colonne %d - Erreur de syntaxe - %s - Symbole trouvé : %s",
          m_lecteur.getLigne(), m_lecteur.getColonne(), message.c_str(), m_lecteur.getSymbole().getChaine().c_str());
  throw SyntaxeException(messageWhat);
}

Noeud* Interpreteur::programme() {
  // <programme> ::= procedure principale() <seqInst> finproc FIN_FICHIER
  testerEtAvancer("procedure");
  testerEtAvancer("principale");
  testerEtAvancer("(");
  testerEtAvancer(")");
  Noeud* sequence = seqInst();
  testerEtAvancer("finproc");
  tester("<FINDEFICHIER>");
  return sequence;
}

Noeud* Interpreteur::seqInst() {
  // <seqInst> ::= <inst> { <inst> }
  NoeudSeqInst* sequence = new NoeudSeqInst();
  do {
    sequence->ajoute(inst());
  } while (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "si" || m_lecteur.getSymbole() == "repeter"
           || m_lecteur.getSymbole() == "tantque" || m_lecteur.getSymbole() == "pour" || m_lecteur.getSymbole() == "ecrire"
            || m_lecteur.getSymbole() == "lire");
  // Tant que le symbole courant est un début possible d'instruction...
  // Il faut compléter cette condition chaque fois qu'on rajoute une nouvelle instruction
  return sequence;
}

Noeud* Interpreteur::inst() {
  // <inst> ::= <affectation>  ; | <instSi>
  try {
    if (m_lecteur.getSymbole() == "<VARIABLE>") {
      Noeud *affect = affectation();
      testerEtAvancer(";");
      return affect;
    }
    else if (m_lecteur.getSymbole() == "si")
      return instSi();
    // Compléter les alternatives chaque fois qu'on rajoute une nouvelle instruction
    else if (m_lecteur.getSymbole() == "repeter")
      return instRepeter();
      else if(m_lecteur.getSymbole() == "tantque")
        return instTantQue();
    else if(m_lecteur.getSymbole() == "pour")
        return instPour();
    else if(m_lecteur.getSymbole() == "ecrire")
        return instEcrire();
    else if (m_lecteur.getSymbole() == "lire")
      return instLire();
    else erreur("Instruction incorrecte");
  } catch (SyntaxeException) {
      cout << "ERREUR : Instruction incorrecte => Erreur traitée => Arbre abstrait vidé" << endl;
      m_arbre = nullptr;
      m_lecteur.avancer();
      inst();
  }
}

Noeud* Interpreteur::affectation() {
  // <affectation> ::= <variable> = <expression> 
  tester("<VARIABLE>");
  Noeud* var = m_table.chercheAjoute(m_lecteur.getSymbole()); // La variable est ajoutée à la table et on la mémorise
  m_lecteur.avancer();
  testerEtAvancer("=");
  Noeud* exp = expression();             // On mémorise l'expression trouvée
  return new NoeudAffectation(var, exp); // On renvoie un noeud affectation
}

Noeud* Interpreteur::expression() {
  // <expression> ::= <facteur> { <opBinaire> <facteur> }
  //  <opBinaire> ::= + | - | *  | / | < | > | <= | >= | == | != | et | ou
  Noeud* fact = facteur();
  while ( m_lecteur.getSymbole() == "+"  || m_lecteur.getSymbole() == "-"  ||
          m_lecteur.getSymbole() == "*"  || m_lecteur.getSymbole() == "/"  ||
          m_lecteur.getSymbole() == "<"  || m_lecteur.getSymbole() == "<=" ||
          m_lecteur.getSymbole() == ">"  || m_lecteur.getSymbole() == ">=" ||
          m_lecteur.getSymbole() == "==" || m_lecteur.getSymbole() == "!=" ||
          m_lecteur.getSymbole() == "et" || m_lecteur.getSymbole() == "ou"   ) {
    Symbole operateur = m_lecteur.getSymbole(); // On mémorise le symbole de l'opérateur
    m_lecteur.avancer();
    Noeud* factDroit = facteur(); // On mémorise l'opérande droit
    fact = new NoeudOperateurBinaire(operateur, fact, factDroit); // Et on construuit un noeud opérateur binaire
  }
  return fact; // On renvoie fact qui pointe sur la racine de l'expression
}

Noeud* Interpreteur::facteur() {
  // <facteur> ::= <entier> | <variable> | - <facteur> | non <facteur> | ( <expression> )
  Noeud* fact = nullptr;
  if (m_lecteur.getSymbole() == "<VARIABLE>" || m_lecteur.getSymbole() == "<ENTIER>") {
    fact = m_table.chercheAjoute(m_lecteur.getSymbole()); // on ajoute la variable ou l'entier à la table
    m_lecteur.avancer();
  } else if (m_lecteur.getSymbole() == "-") { // - <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustraction binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("-"), m_table.chercheAjoute(Symbole("0")), facteur());
  } else if (m_lecteur.getSymbole() == "non") { // non <facteur>
    m_lecteur.avancer();
    // on représente le moins unaire (- facteur) par une soustractin binaire (0 - facteur)
    fact = new NoeudOperateurBinaire(Symbole("non"), facteur(), nullptr);
  } else if (m_lecteur.getSymbole() == "(") { // expression parenthésée
    m_lecteur.avancer();
    fact = expression();
    testerEtAvancer(")");
  } else
    erreur("Facteur incorrect");
  return fact;
}

Noeud* Interpreteur::instSi() {
  // <instSi> ::= si ( <expression> ) <seqInst> finsi
    
  testerEtAvancer("si");
  testerEtAvancer("(");
  Noeud* condition = expression(); // On mémorise la condition
  testerEtAvancer(")");
  Noeud* sequence = seqInst();     // On mémorise la séquence d'instruction
  if(m_lecteur.getSymbole() == "sinonsi" || m_lecteur.getSymbole() == "sinon"){
    return instSiRiche(condition,sequence);
  }else{
    testerEtAvancer("finsi");
    return new NoeudInstSi(condition, sequence); // Et on renvoie un noeud Instruction Si   
  }
}

Noeud* Interpreteur::instRepeter(){
    testerEtAvancer("repeter");
    Noeud* sequence = seqInst();
    testerEtAvancer("jusqua");
    testerEtAvancer("(");
    Noeud* condition = expression();
    testerEtAvancer(")");
    return new NoeudInstRepeter(sequence,condition);
}

 Noeud* Interpreteur::instEcrire() {
    testerEtAvancer("ecrire");
    testerEtAvancer("(");
    /*m_table.chercheAjoute("<CHAINE>");
    if (m_lecteur.getSymbole() == "<CHAINE>")
     */
    return nullptr;
 } 
Noeud* Interpreteur::instTantQue() {
    testerEtAvancer("tantque");
    testerEtAvancer("(");
    Noeud* condition = expression();
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("fintantque");
    return new NoeudInstTantQue(condition,sequence);
}
Noeud* Interpreteur::instSiRiche(Noeud* condition, Noeud* sequence) {
    vector<Noeud*> conditions;
    vector<Noeud*> sequences;
    conditions.push_back(condition);
    sequences.push_back(sequence);
    while(m_lecteur.getSymbole() == "sinonsi"){
        testerEtAvancer("sinonsi");
        testerEtAvancer("(");
        Noeud* condition1 = expression();
        conditions.push_back(condition1);
        testerEtAvancer(")");
        Noeud* sequence1 = seqInst();
        sequences.push_back(sequence1);

    }
    if(m_lecteur.getSymbole() == "sinon"){
        testerEtAvancer("sinon");
        Noeud * sequence1 = seqInst();
        cout << sequence1 << endl;
        conditions.push_back(sequence1);
        sequences.push_back(sequence1);
    }
    testerEtAvancer("finsi");
    return new NoeudInstSiRiche(conditions,sequences);
}
Noeud* Interpreteur::instPour() {
    testerEtAvancer("pour");
    testerEtAvancer("(");
    Noeud* affectation1 = NULL;
    Noeud* affectation2 = NULL;
    if(!(m_lecteur.getSymbole() == ";")){
       affectation1 = affectation();
    }
    testerEtAvancer(";");
    Noeud* condition = expression();
    testerEtAvancer(";");
    if(!(m_lecteur.getSymbole() == ")")){
        affectation2 = affectation();
    }
    testerEtAvancer(")");
    Noeud* sequence = seqInst();
    testerEtAvancer("finpour");
    return new NoeudInstPour(condition,sequence,affectation1,affectation2);
}

Noeud* Interpreteur::instLire() {
    testerEtAvancer("lire");
    testerEtAvancer("(");
    tester("<VARIABLE>");
    vector<Noeud*> variables;
    variables.push_back(facteur());
    while (m_lecteur.getSymbole()== ",") {
        testerEtAvancer("<VARIABLE>");
        variables.push_back(facteur());
    }
    testerEtAvancer(")");
    return new NoeudInstLire(variables);
 }
//      <instEcrire> ::= ecrire ( <expression> | <chaine> { , <expression> | <chaine> } )
Noeud* Interpreteur::instEcrire() {
    testerEtAvancer("ecrire");
    testerEtAvancer("(");
    vector<Noeud*> v;
    do{
        Noeud* ve;
        if(m_lecteur.getSymbole() == "<CHAINE>"){
          ve= new SymboleValue(m_lecteur.getSymbole().getChaine());
          m_lecteur.avancer();
        }else{
          ve = expression();
        }
        v.push_back(ve);
        if(m_lecteur.getSymbole() == ","){
            testerEtAvancer(",");
        }
    }while(m_lecteur.getSymbole() == "<CHAINE>" || m_lecteur.getSymbole() == "<VARIABLE>" );
    testerEtAvancer(")");
    return new NoeudInstEcrire(v);
}
