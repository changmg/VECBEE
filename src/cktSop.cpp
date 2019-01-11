#include "cktSop.h"


using namespace std;
using namespace abc;


Ckt_Sop_t::Ckt_Sop_t(Abc_Obj_t * p_abc_obj, Ckt_Sop_Net_t * p_ckt_ntk)
    : pAbcObj(p_abc_obj), pCktNtk(p_ckt_ntk), type(Abc_GetSopType(p_abc_obj)), isVisited(false), topoId(0)
{
    valueClusters.resize(pCktNtk->GetValClustersNum());
    valueClustersBak.resize(pCktNtk->GetValClustersNum());
    foConeInfo.resize((Abc_NtkPoNum(pCktNtk->GetAbcNtk()) >> 6) + 1);
    CollectPCN();
    BD.resize(Abc_NtkPoNum(pCktNtk->GetAbcNtk()));
}


Ckt_Sop_t::Ckt_Sop_t(const Ckt_Sop_t & other)
    : pAbcObj(other.pAbcObj), pCktNtk(other.pCktNtk), type(other.GetType()), isVisited(other.isVisited), topoId(other.topoId)
{
    // shallow copy
    valueClusters.resize(other.pCktNtk->GetValClustersNum());
    valueClustersBak.resize(other.pCktNtk->GetValClustersNum());
    foConeInfo.resize(other.foConeInfo.size());
    PCN.assign(other.PCN.begin(), other.PCN.end());
    BD.resize(other.BD.size());
}


Ckt_Sop_t::~Ckt_Sop_t(void)
{
}


void Ckt_Sop_t::PrintFanios(void) const
{
    string temp = "";
    for (auto & pCktFanin : pCktFanins) {
        temp += pCktFanin->GetName();
        temp += ", ";
    }
    cout << setw(30) << setiosflags(ios::left) << temp;
    temp = "";
    for (auto & pCktFanout : pCktFanouts) {
        temp += pCktFanout->GetName();
        temp += ", ";
    }
    cout << setw(30) << setiosflags(ios::left) << temp;
}


void Ckt_Sop_t::PrintPCN(void) const
{
    for (auto s : PCN)
        cout << s << " ";
}


void Ckt_Sop_t::CollectPCN(void)
{
    if (IsPI() || IsPO() || IsConst())
        return;
    char * pCube, * pSop = (char *)pAbcObj->pData;
    int Value, v;
    assert(pSop && !Abc_SopIsExorType(pSop));
    int nVars = Abc_SopGetVarNum(pSop);
    PCN.clear();
    Abc_SopForEachCube(pSop, nVars, pCube) {
        string s = "";
        Abc_CubeForEachVar(pCube, Value, v)
            if (Value == '0' || Value == '1' || Value == '-')
                s += static_cast<char>(Value);
            else
                continue;
        PCN.emplace_back(s);
    }
}


void Ckt_Sop_t::PrintClusters(void) const
{
    for (auto & cluster : valueClusters) {
        for (int i = 0; i < 64; ++i) {
            cout << Ckt_GetBit(cluster, static_cast <uint64_t> (i));
        }
    }
}


void Ckt_Sop_t::UpdateClusters(void)
{
}


ostream & operator << (ostream & os, const Ckt_Sop_Cat_t & type)
{
    switch ( type ) {
        case Ckt_Sop_Cat_t::PI:
            cout << "PI";
        break;
        case Ckt_Sop_Cat_t::PO:
            cout << "PO";
        break;
        case Ckt_Sop_Cat_t::CONST0:
            cout << "CONST0";
        break;
        case Ckt_Sop_Cat_t::CONST1:
            cout << "CONST1";
        break;
        case Ckt_Sop_Cat_t::INTER:
            cout << "INTER";
        break;
        default:
            assert(0);
    }
    return os;
}


Ckt_Sop_Cat_t Abc_GetSopType( Abc_Obj_t * pObj )
{
    if (Abc_ObjIsPi(pObj))
        return Ckt_Sop_Cat_t::PI;
    if (Abc_ObjIsPo(pObj))
        return Ckt_Sop_Cat_t::PO;
    assert(Abc_ObjIsNode(pObj));
    if ( Abc_NodeIsConst0(pObj) )
        return Ckt_Sop_Cat_t::CONST0;
    else if ( Abc_NodeIsConst1(pObj) )
        return Ckt_Sop_Cat_t::CONST1;
    else
        return Ckt_Sop_Cat_t::INTER;
}