#include <stdlib.h>
#include "ArbreAbstrait.h"
#include "Symbole.h"
#include "SymboleValue.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////
// NoeudSeqInst
////////////////////////////////////////////////////////////////////////////////

NoeudSeqInst::NoeudSeqInst() : m_instructions() {
}

int NoeudSeqInst::executer() {
  for (unsigned int i = 0; i < m_instructions.size(); i++)
    m_instructions[i]->executer(); // on exécute chaque instruction de la séquence
  return 0; // La valeur renvoyée ne représente rien !
}

void NoeudSeqInst::ajoute(Noeud* instruction) {
  if (instruction!=nullptr) m_instructions.push_back(instruction);
}

////////////////////////////////////////////////////////////////////////////////
// NoeudAffectation
////////////////////////////////////////////////////////////////////////////////

NoeudAffectation::NoeudAffectation(Noeud* variable, Noeud* expression)
: m_variable(variable), m_expression(expression) {
}

int NoeudAffectation::executer() {
  int valeur = m_expression->executer(); // On exécute (évalue) l'expression
  ((SymboleValue*) m_variable)->setValeur(valeur); // On affecte la variable
  return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudOperateurBinaire
////////////////////////////////////////////////////////////////////////////////

NoeudOperateurBinaire::NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit)
: m_operateur(operateur), m_operandeGauche(operandeGauche), m_operandeDroit(operandeDroit) {
}

int NoeudOperateurBinaire::executer() {
  int og, od, valeur;
  if (m_operandeGauche != nullptr) og = m_operandeGauche->executer(); // On évalue l'opérande gauche
  if (m_operandeDroit != nullptr) od = m_operandeDroit->executer(); // On évalue l'opérande droit
  // Et on combine les deux opérandes en fonctions de l'opérateur
  if (this->m_operateur == "+") valeur = (og + od);
  else if (this->m_operateur == "-") valeur = (og - od);
  else if (this->m_operateur == "*") valeur = (og * od);
  else if (this->m_operateur == "==") valeur = (og == od);
  else if (this->m_operateur == "!=") valeur = (og != od);
  else if (this->m_operateur == "<") valeur = (og < od);
  else if (this->m_operateur == ">") valeur = (og > od);
  else if (this->m_operateur == "<=") valeur = (og <= od);
  else if (this->m_operateur == ">=") valeur = (og >= od);
  else if (this->m_operateur == "et") valeur = (og && od);
  else if (this->m_operateur == "ou") valeur = (og || od);
  else if (this->m_operateur == "non") valeur = (!og);
  else if (this->m_operateur == "/") {
    if (od == 0) throw DivParZeroException();
    valeur = og / od;
  }
  return valeur; // On retourne la valeur calculée
}

////////////////////////////////////////////////////////////////////////////////
// NoeudInstSi
////////////////////////////////////////////////////////////////////////////////

NoeudInstSi::NoeudInstSi(Noeud* condition, Noeud* sequence)
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstSi::executer() {
  if (m_condition->executer()) m_sequence->executer();
  return 0; // La valeur renvoyée ne représente rien !
}

//////////////////////////////////////////////////////////////////
/// NoeudInstRepeter
//////////////////////////////////////////////////////////////////

NoeudInstRepeter::NoeudInstRepeter(Noeud* sequence, Noeud* condition)
: m_sequence(sequence), m_condition(condition) {   
}

int NoeudInstRepeter::executer() {
    while (!(m_condition->executer())) m_sequence->executer();
    return 0;
}

////////////////////////////////////////////////////////////////////////////////
// NoeudTantque
////////////////////////////////////////////////////////////////////////////////

NoeudInstTantQue::NoeudInstTantQue(Noeud* condition, Noeud* sequence) 
: m_condition(condition), m_sequence(sequence) {
}

int NoeudInstTantQue::executer() {
    while(m_condition->executer()) m_sequence->executer();
    return 0; // La valeur renvoyée ne représente rien !
}

////////////////////////////////////////////////////////////////////////////////
// NoeudSiRiche
////////////////////////////////////////////////////////////////////////////////

NoeudInstSiRiche::NoeudInstSiRiche(vector<Noeud*>  conditions,vector<Noeud*>  sequences)
:m_conditions(conditions),m_sequences(sequences){
}

int NoeudInstSiRiche::executer() {
    unsigned i = 0 ;  // Indice pour parcourir le vector
                            //boolean pour sortir de la boucle
    bool sortie = false;
    while(i<m_conditions.size() && !sortie){    // Tant qu'il reste des conditions et qu'aucune n'a était réaliser faire
        if(m_conditions.at(i) == m_sequences.at(i)){ // si la condition vaut NULL alors c'est un sinon donc faire puis sortir de la boucle
            m_sequences.at(i)->executer();
            sortie = true;
        }
        if(m_conditions.at(i)->executer()) {         // si la condition est vrai alors faire puis sortir de la boucle
            m_sequences.at(i)->executer();
            sortie = true;
        }
        i++;
    }

    return 0;
}

////////////////////////////////////////////////////////////////////////////////
//NoeudInstPour
////////////////////////////////////////////////////////////////////////////////

NoeudInstPour::NoeudInstPour(Noeud* condition, Noeud* sequence, Noeud* affectation1, Noeud* affectation2)
:m_condition(condition),m_sequence(sequence),m_affectation1(affectation1),m_affectation2(affectation2){

}

int NoeudInstPour::executer() {
    for(m_affectation1 != NULL ? m_affectation1->executer() : 0;m_condition->executer();m_affectation2 != NULL ? m_affectation2->executer() : 0){
        m_sequence->executer();
    }
}

//////////////////////////////////////////////////////////////////
/// NoeudInstLire
//////////////////////////////////////////////////////////////////

NoeudInstLire::NoeudInstLire(vector<Noeud*> variables) 
: m_variables(variables) {
}

int NoeudInstLire::executer() {
    for (int i=0; i<m_variables.size(); i++) {
        cout << ((SymboleValue*) m_variables[i])->getValeur() << endl;
    }
    return 0;
}
