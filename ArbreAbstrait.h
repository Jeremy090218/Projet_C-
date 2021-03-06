#ifndef ARBREABSTRAIT_H
#define ARBREABSTRAIT_H

// Contient toutes les déclarations de classes nécessaires
//  pour représenter l'arbre abstrait

#include <vector>
#include <iostream>
#include <iomanip>
using namespace std;

#include "Symbole.h"
#include "Exceptions.h"

////////////////////////////////////////////////////////////////////////////////
class Noeud {
// Classe abstraite dont dériveront toutes les classes servant à représenter l'arbre abstrait
// Remarque : la classe ne contient aucun constructeur
  public:
    virtual int  executer() =0 ; // Méthode pure (non implémentée) qui rend la classe abstraite
    virtual void ajoute(Noeud* instruction) { throw OperationInterditeException(); }
    virtual ~Noeud() {} // Présence d'un destructeur virtuel conseillée dans les classes abstraites
};

////////////////////////////////////////////////////////////////////////////////
class NoeudSeqInst : public Noeud {
// Classe pour représenter un noeud "sequence d'instruction"
//  qui a autant de fils que d'instructions dans la séquence
  public:
     NoeudSeqInst();   // Construit une séquence d'instruction vide
    ~NoeudSeqInst() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();    // Exécute chaque instruction de la séquence
    void ajoute(Noeud* instruction);  // Ajoute une instruction à la séquence

  private:
    vector<Noeud *> m_instructions; // pour stocker les instructions de la séquence
};

////////////////////////////////////////////////////////////////////////////////
class NoeudAffectation : public Noeud {
// Classe pour représenter un noeud "affectation"
//  composé de 2 fils : la variable et l'expression qu'on lui affecte
  public:
     NoeudAffectation(Noeud* variable, Noeud* expression); // construit une affectation
    ~NoeudAffectation() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();        // Exécute (évalue) l'expression et affecte sa valeur à la variable

  private:
    Noeud* m_variable;
    Noeud* m_expression;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudOperateurBinaire : public Noeud {
// Classe pour représenter un noeud "opération binaire" composé d'un opérateur
//  et de 2 fils : l'opérande gauche et l'opérande droit
  public:
    NoeudOperateurBinaire(Symbole operateur, Noeud* operandeGauche, Noeud* operandeDroit);
    // Construit une opération binaire : operandeGauche operateur OperandeDroit
   ~NoeudOperateurBinaire() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();            // Exécute (évalue) l'opération binaire)

  private:
    Symbole m_operateur;
    Noeud*  m_operandeGauche;
    Noeud*  m_operandeDroit;
};

////////////////////////////////////////////////////////////////////////////////
class NoeudInstSi : public Noeud {
// Classe pour représenter un noeud "instruction si"
//  et ses 2 fils : la condition du si et la séquence d'instruction associée
  public:
    NoeudInstSi(Noeud* condition, Noeud* sequence);
     // Construit une "instruction si" avec sa condition et sa séquence d'instruction
   ~NoeudInstSi() {} // A cause du destructeur virtuel de la classe Noeud
    int executer();  // Exécute l'instruction si : si condition vraie on exécute la séquence

  private:
    Noeud*  m_condition;
    Noeud*  m_sequence;
};
//////////////////////////////////////////////////////////////////////
class NoeudInstRepeter : public Noeud {
    // Classe pour représenter un noeud "repeter"
    // et ses 2 fils : la séquence d'instructions à retenir et la condition de fin de boucle
public:
    NoeudInstRepeter(Noeud* sequence, Noeud* condition);
    // Construit une "instruction repeter" avec sa condition et sa séquence d'instruction    
    ~NoeudInstRepeter() {}
    int executer(); // Exécute l'instruction repeter : tant que condition fausse on exécute la séquence
private:
    Noeud* m_sequence;
    Noeud* m_condition;
};

class NoeudInstTantQue : public Noeud {
// Classe pour représenter un noeud "instruction si"
// et ses 2 fils : la condition du si et la séquence d'intruction associée
  public:
      NoeudInstTantQue(Noeud* condition, Noeud* sequence);
       // Construit une "instruction tanque" avec sa condition et sa séquence d'instruction
    ~NoeudInstTantQue() {} // A cause du destructeur virtuel de la classe Noeud
    int executer(); //Exécute l'instruction tantque : tant que la condition est vraie on exécute la séquence
    
  private:
      Noeud* m_condition;
      Noeud* m_sequence;
};

class NoeudInstSiRiche : public Noeud {
// Classe pour représenter un noeud "instruction si avec plusieurs sinonsi et/ou un sinon"
// et ses filles : vector de plusieurs Si
    
  public:
      NoeudInstSiRiche(vector<Noeud*>  conditions,vector<Noeud*>  sequences);
        //Construit un tableau "instruction si"
      ~NoeudInstSiRiche(){} // A cause du destructeur virtuel de la classe Noeud
      int executer(); //Exécute l'instruction tantque : tant que la condition est vraie on exécute la séquence
      
  private:
      vector<Noeud*>  m_conditions;
      vector<Noeud*>  m_sequences;
};

class NoeudInstPour : public Noeud {
    
public:
    NoeudInstPour(Noeud* condition,Noeud* sequence,Noeud* affectation1,Noeud* affectation2);
    ~NoeudInstPour(){}
    int executer();
    
private:
    Noeud* m_condition;
    Noeud* m_sequence;
    Noeud* m_affectation1;
    Noeud* m_affectation2;
};

//////////////////////////////////////////////////////////////////////
class NoeudInstLire : public Noeud {
    // Classe pour représenter un noeud "lire"
    // et son nombre de variables filles aléatoire (1..*) 
public:
    NoeudInstLire(vector<Noeud*> variables);
    // Construit une "instruction lire" avec sa liste de variables
    ~NoeudInstLire() {}
    int executer(); //Exécute l'instruction lire : affiche la valeur de chaque variable de la liste
private:
    vector<Noeud*> m_variables;
};

///////////////////////////////////////////////////////////////////////
class NoeudInstEcrire : public Noeud {
public:
    NoeudInstEcrire(vector<Noeud*>s);
    ~NoeudInstEcrire(){}
    int executer();
    
private:
    vector<Noeud*> m_s;
};


#endif /* ARBREABSTRAIT_H */
