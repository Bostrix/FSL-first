/*    Copyright (C) 2012 University of Oxford  */

/*  CCOPYRIGHT  */


#ifndef _fslvtkIO
#define _fslvtkIO

#include <iostream>

#include <string>
#include <fstream>
#include <stdio.h>
#include <algorithm>

#include "armawrap/newmat.h"
#include "meshclass/meshclass.h"
#include "first_lib/first_newmat_vec.h"


namespace fslvtkio{
  //using namespace mesh;

  class fslvtkIOException : public std::exception{

  public:
    const char* errmesg;
    fslvtkIOException(const char* msg)
    {
      errmesg=msg;
    }

  private:
    virtual const char* what() const throw()
    {
      std::cout<<errmesg<<std::endl;
      return errmesg;
    }
  };

  class fslvtkIO {
  public:
    enum DataType{ POLYDATA, UNSTRUCTURED_GRID };
    fslvtkIO();
    fslvtkIO(const std::string & filename,const DataType i);

    ~fslvtkIO();


    //----------------------SET COMMANDS-------------------------//
    void setPoints(const NEWMAT::Matrix & pts);
    void setPoints(const std::vector<float> & pts);
    void setCells(const std::vector< std::vector<unsigned int> > & c){ Cells=c; }
    void setCells(const std::vector< std::vector<unsigned int> > & c, const std::vector<short> & c_t ){ Cells=c; Cell_Types=c_t; }

    void appendPointsAndPolygons(const NEWMAT::Matrix & pts, const NEWMAT::Matrix & Polygons);


    void setPolygons(const NEWMAT::Matrix& m){ Polygons=m; }
    void setPolygons(const std::vector< std::vector<unsigned int> >& vm);
    void setMesh(const mesh::Mesh & m);
    //void setScalars(const Matrix & sc,const string & name);
    void setScalars(const NEWMAT::Matrix& m){ Scalars=m; }

    void setVectors(const NEWMAT::Matrix & vecs,const std::string &name);
    void setVectors(const NEWMAT::Matrix& m){ Vectors=m; }

    template<class T>
    void setScalars(const std::vector<T> & sc);


    void addPointFieldData(const NEWMAT::Matrix& m,const std::string & name, const std::string & type, const std::string & vtkAttType);
    void addCellFieldData(const NEWMAT::Matrix& m,const std::string & name, const std::string & type, const std::string & vtkAttType);

    void addFieldData(const NEWMAT::Matrix& m,const std::string & name, const std::string & type);

    template< class T >
    void addFieldData(const std::vector<T> & m,const std::string & name, const std::string & type);

    template< class T >
    void addFieldData(const T & m,const std::string & name, const std::string & type);

    void replaceFieldData(const NEWMAT::Matrix& m,const std::string & name);

    void addFieldData(std::vector< std::string >,std::string name);


    void printFieldDataNames() { for (std::vector<std::string>::iterator i=fieldDataNumName.begin(); i!=fieldDataNumName.end();i++) std::cout<<"field "<<*i<<std::endl; }


    //----------------------GET COMMANDS----------------------------//
    NEWMAT::Matrix getPointsAsMatrix() const { return Points; }
    NEWMAT::ColumnVector getPointsAsColumnVector() const ;



    template<class T>
    std::vector<T> getPointsAsVector();

    template<class T>
    std::vector< std::vector<T> > getPointsAsVectorOfVectors(){ return FIRST_LIB::first_newmat_vector::matrixToVectorOfVectors<T>(Points); }

    template<class T>
    std::vector< std::vector<T> > getPolygonsAsVectorOfVectors(){ return FIRST_LIB::first_newmat_vector::matrixToVectorOfVectors<T>(Polygons); }

    NEWMAT::ReturnMatrix getPolygons() const { return Polygons; };
    std::vector< std::vector<unsigned int> > getCells() const { return Cells; }
    std::vector<short> getCellTypes() const { return Cell_Types; }

    std::string getFieldName(const int & ind) const { return fieldDataNumName.at(ind); };
    unsigned int getNumberOfFields() const { return fieldDataNumName.size(); };
    NEWMAT::Matrix getField(const std::string & name);
    NEWMAT::Matrix getField(const std::string & name, unsigned int & ind);
    void setField(const std::string & name, const NEWMAT::Matrix & data);

    NEWMAT::ReturnMatrix getScalars(){ return Scalars; }

    template<class T>
    std::vector<T> getScalars();


    NEWMAT::ReturnMatrix getVectors(){ return Vectors; }


    //----------------------I/O COMMANDS----------------------------//
    template<class T>
    void writePoints(std::ofstream & fshape, const std::string & str_typename);
    void writePolygons(std::ofstream & fshape);
    void writeCells(std::ofstream & fshape);
    //void writeUnstructuredGridCells(ofstream & fshape);
    void writeUnstructuredGridCellTypes(std::ofstream & fshape);
    void save(std::string s);

    void readUnstructuredGrid(std::string fname);
    void readPolyData(std::string fname);
    template<class T>
    void writePointData(std::ofstream & fshape, const std::string & str_typename );

    template <class T>
    void writeNumericField(std::ofstream & fvtk, const std::string & name, const std::string & type, const NEWMAT::Matrix & Data);
    void writeStringField(std::ofstream & fvtk, const std::string & name, const std::vector<std::string> & v_string);

    void readFieldData(std::ifstream & fvtk);
    void readPointData(std::ifstream & fvtk, std::string & nextData);
    bool  readPoints(std::ifstream & fvtk);
    bool  readPolygons(std::ifstream & fvtk);

    template <class T>
    NEWMAT::ReturnMatrix readField(std::ifstream & fvtk, const int & nrows,const int & mcols);

    void displayNumericFieldDataNames();
    void displayNumericField(const std::string & name);

    //----------------------SETTING/ACCESS OF STATE VARIABLES----------------------------//
    void setSwitchRowsCols(bool n) { SWITCH_ROWS_COLS=n; }

    void setMAX(bool b) { MAX_SET=b; }
    void setMAX_Val( unsigned int n ) { MAX=n; }
    void setDataType(const DataType & dtype ){ dt=dtype; }
    void setBinaryWrite(bool state) { BINARY=state; }
    bool getBinaryWrite() { return BINARY; }
    static bool myExceptions(int e);

  protected:
    NEWMAT::Matrix Scalars;
    NEWMAT::Matrix Vectors;
    NEWMAT::Matrix Points;
    //cell data
    NEWMAT::Matrix Polygons;



  private:
#define BINFLAG 42
    //----------------------STATE VARIABLES/CONSTANTS----------------------------//

#ifdef PPC64
	int m_n;
#endif
    bool BINARY;//state variable for read write
    bool SWAP_BYTES;//only used for binary read
    bool MAX_SET; //is a max number of columns imposed
    bool SWITCH_ROWS_COLS;//SWITCHES THE COLUMN/ROWS , USED FOR FIXING OLD FILES

    unsigned int ST_COUNT;

    unsigned int MAX;
    DataType dt;//i.e. POLYDATA




    //----------------------READ COMMANDS----------------------------//

    //----------------------DATA----------------------------//

    //point data
    std::string scalarsName, vectorsName;

    std::vector< std::vector<unsigned int> > Cells;
    std::vector<short> Cell_Types;

    std::string cd_scalarsName, cd_vectorsName;
    NEWMAT::Matrix cd_Scalars;
    NEWMAT::Matrix cd_Vectors;


    //field data
    std::vector< NEWMAT::Matrix > fieldDataNum;
    std::vector< std::string > fieldDataNumName;
    std::vector< std::string > fieldDataNumType;

    std::vector< std::vector<std::string> > fieldDataStr;
    std::vector< std::string > fieldDataStrName;

    //defaults FIELDData
    std::vector< std::string > pd_list;
    std::vector< std::string > pd_type;

    std::vector< std::string > cd_list;
    std::vector< std::string > cd_type;

  };
}
#endif
