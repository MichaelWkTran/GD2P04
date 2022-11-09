/************************************************************************************************************************/
/*	NAME: Michael Wai Kit Tran																							*/
/*	ORGN: Bachelor of Software Engineering, Media Design School															*/
/*	FILE: TinyObjectLoader.h																							*/
/*  DATE: Aug 23rd, 2022																								*/
/************************************************************************************************************************/

#pragma once
#include "../Include/TinyObjLoader/tiny_obj_loader.h"
#include "Mesh.h"

void GetObjModelData(CMesh<stVertex>& _Mesh, const char* _pOBJModelDirectory);