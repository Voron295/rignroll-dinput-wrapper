#include "D3DRenderData.h"
extern IDirect3DDevice9 *&g_pDirect3DDevice;

extern DWORD &g_bCanBlendMaterials;
extern DWORD &g_bCanBlendCurrentAlpha;
extern DWORD &g_bCanSelectArg1;
extern DWORD &g_bCanSelectArg2;
extern DWORD &g_bCanBlendFactorAlpha;
extern DWORD &g_dwPrevMaterialHadAlpha;
extern DWORD &g_bCanModulate2X;

extern Material *&g_pPrevMaterialApplied;
extern int &g_pPrevMaterialAppliedFlag;
extern int g_dwPrevMaterialUsedTransparency;
extern DWORD &g_dwPrevMaterialTexId;

extern DWORD &gRenderFlags;

void D3DRenderData::SetRenderMaterial(int matId, int flag1, int flag2)
{
	ApplyMaterial(&m_pMaterials[matId], flag1, flag2, 0);
}

void D3DRenderData::ApplyMaterial(Material *applyMat, int flag1, int flag2, int flag3)
{
	int v1 = flag3;
	if (!field_21D8)
	{
		while (!field_21DC)
		{
			Material *mat = applyMat;
			int parId;
			if (v1 > field_2054 || (parId = applyMat->m_dwParentId_, parId < 0))
			{
				if (g_pPrevMaterialApplied != applyMat || flag1 != g_pPrevMaterialAppliedFlag)
				{
					GraphicsPalette *pal = m_pGraphicsData->GetPalette();
					if (pal)
					{
						RGB color = pal->palette[mat->m_dwColorId];
						int v2 = 1;
						D3DMATERIAL9 d3dMat;
						memset(&d3dMat, 0, sizeof(D3DMATERIAL9));


						float colR, colG, colB, colA;
						float specR, specG, specB, specA;
						float transp = (1.0 - mat->m_fTransparency) * (double)field_20F8 * 0.0039215689;
						if (transp > 0.99)
							transp = 1.0;

						colR = color.r * 0.00390625;
						colG = color.g * 0.00390625;
						colB = color.b * 0.00390625;
						colA = transp;

						if (mat->flags & MATERIAL_HAS_TEXTURE)
						{
							if (g_pPrevMaterialApplied
								&& g_pPrevMaterialApplied->flags == mat->flags
								&& g_pPrevMaterialApplied->m_dwPower == mat->m_dwPower
								&& flag1 == g_pPrevMaterialAppliedFlag
								&& !(mat->flags & MATERIAL_USE_COL))
							{
								v2 = 0;
							}

							colA = transp;
							if (flag2) // color
							{
								switch (flag2)
								{
								case 1:
									colR = 1.0;
									colG = 0.0;
									colB = 0.0;
									break;

								case 2:
									colR = 0.0;
									colG = 0.0;
									colB = 1.0;
									break;

								case 3:
									colR = 0.0;
									colG = 0.0;
									colB = 1.0;
									break;
								}
							}
							else if (mat->flags & MATERIAL_USE_COL)
							{
								colR = color.r * 0.00390625;
								colG = color.g * 0.00390625;
								colB = color.b * 0.00390625;
							}
							else
							{
								colR = 1.0;
								colG = 1.0;
								colB = 1.0;
							}
						}

						specR = mat->m_fSpecular;
						specG = mat->m_fSpecular;
						specB = mat->m_fSpecular;

						int v3 = SetupMaterialsWithBlending(mat, g_pPrevMaterialApplied, 0);
						int v4 = 1;
						if (v3 == g_dwPrevMaterialUsedTransparency)
							v4 = v2;
						g_dwPrevMaterialUsedTransparency = v3;

						bUsingTexture = 0; // skipped some code

						if (v4)
						{
							if (mat->flags & MATERIAL_IS_TRANSPARENT || field_20F8 != -1 || v3)
								g_pDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 1);
							else
								g_pDirect3DDevice->SetRenderState(D3DRS_ALPHABLENDENABLE, 0);

							d3dMat.Diffuse.r = colR;
							d3dMat.Diffuse.g = colG;
							d3dMat.Diffuse.b = colB;
							d3dMat.Diffuse.a = colA;
							d3dMat.Specular.r = specR;
							d3dMat.Specular.g = specG;
							d3dMat.Specular.b = specB;
							d3dMat.Specular.a = 0.0;
							d3dMat.Emissive.r = 0.0;
							d3dMat.Emissive.g = 0.0;
							d3dMat.Emissive.b = 0.0;
							d3dMat.Emissive.a = 0.0;
							d3dMat.Power = (double)mat->m_dwPower;
							if (flag1)
							{
								d3dMat.Ambient.r = colR;
								d3dMat.Ambient.g = colG;
								d3dMat.Ambient.b = colB;
								d3dMat.Ambient.a = colA;

								g_pDirect3DDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, 0);
								g_pDirect3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, 0);
							}
							else
							{
								d3dMat.Ambient.r = 0.0;
								d3dMat.Ambient.g = 0.0;
								d3dMat.Ambient.b = 0.0;
								d3dMat.Ambient.a = 0.0;

								g_pDirect3DDevice->SetRenderState(D3DRS_EMISSIVEMATERIALSOURCE, 1);
								g_pDirect3DDevice->SetRenderState(D3DRS_DIFFUSEMATERIALSOURCE, 1);
							}
							g_pDirect3DDevice->SetMaterial(&d3dMat);

							// skip some fog data

							if (gRenderFlags & 0x4000) // zbuf enabled
							{
								if (!(mat->flags & MATERIAL_NOZ))
								{
									if (mat->flags & 0x40000)
									{
										g_pDirect3DDevice->SetRenderState(D3DRS_ZENABLE, 1);
										g_pDirect3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, 0);
										g_pDirect3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
										g_pPrevMaterialApplied = mat;
										g_pPrevMaterialAppliedFlag = flag1;
										return;
									}
									g_pDirect3DDevice->SetRenderState(D3DRS_ZENABLE, 1);
									g_pDirect3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, 1);
									g_pDirect3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_LESSEQUAL);
								}
								else
								{
									g_pDirect3DDevice->SetRenderState(D3DRS_ZENABLE, 0);
									g_pDirect3DDevice->SetRenderState(D3DRS_ZWRITEENABLE, 0);
									g_pDirect3DDevice->SetRenderState(D3DRS_ZFUNC, D3DCMP_ALWAYS);
								}
							}
						}
						g_pPrevMaterialApplied = mat;
						g_pPrevMaterialAppliedFlag = flag1;
					}
				}
				return;
			}
			v1++;
			mat = &m_pMaterials[parId];
		}
	}
}

float g_matPosX = 50.0f;
float g_matPosY = 20.0f;
float g_matScaleX = 0.01;
float g_matScaleY = 0.04;

int D3DRenderData::SetupMaterialsWithBlending(Material *mat1, Material *mat2, DWORD texId)
{
	// skip has bump

	int hasAlpha = 0;
	if (!(mat1->flags & MATERIAL_HAS_TEXTURE))
	{
		g_pDirect3DDevice->SetTexture(0, 0);
		if (mat1->flags == 0) 
		{
			g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTOP_SELECTARG1);
			g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_ALPHAARG1, D3DTA_DIFFUSE);
		}
		else
		{
			g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_COLOROP, D3DTA_CURRENT);
		}
			
		g_pDirect3DDevice->SetTextureStageState(0, D3DTSS_ALPHAOP, D3DTA_CURRENT);
		return 0;
	}
	Texture *tex = mat1->m_pTexture;
	if (tex->m_dTextureFormat == 5 && !(mat1->flags & MATERIAL_HAS_ALPHAMIR) && g_dwPrevMaterialTexId == 0)
		hasAlpha = 1;
	
	int setTexId = texId;
	int setCoordsId = mat1->m_dwInternalTexCoordsId;
	g_pDirect3DDevice->SetTexture(setTexId, tex->m_pSurface);
	g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_TEXCOORDINDEX, setCoordsId);
	if (mat1->flags & MATERIAL_HAS_REFLECTION)
	{
		float *translPosX = &g_matPosX;
		g_pDirect3DDevice->SetRenderState(D3DRS_TEXTUREFACTOR, D3DCOLOR_ARGB((int)(mat1->m_fReflection * 255.0), 0, 0, 0));
		//g_pDirect3DDevice->SetRenderState(D3DRS_WRAP1, D3DWRAP_V | D3DWRAP_U);
		D3DXMATRIX m_matRefl = D3DXMATRIXA16(1.0f, 0.0f, 0.0f, 0.0f,
			0.0f, 1.0f, 0.0f, 0.0f,
			0.0f, 0.0f, 1.0f, 0.0f,
			g_matPosX, g_matPosY, 0.0f, 1.0f);
		D3DXMATRIX view, out, scale;
		g_pDirect3DDevice->GetTransform(D3DTS_VIEW, &view);
		D3DXMatrixInverse(&out, 0, &view);
		D3DXMatrixMultiply(&view, &m_matRefl, &out);

		
		D3DXMatrixScaling(&scale, g_matScaleX, g_matScaleY, 1.0);
		m_matRefl *= scale;
		g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_TEXCOORDINDEX, D3DTSS_TCI_CAMERASPACEPOSITION | 1);
		g_pDirect3DDevice->SetTransform(D3DTS_TEXTURE1, &m_matRefl);
		g_pDirect3DDevice->SetSamplerState(setTexId, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		g_pDirect3DDevice->SetSamplerState(setTexId, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);

		g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_TEXTURETRANSFORMFLAGS, D3DTTFF_COUNT3);
		
	}

	if (!(mat2 && mat1->flags == mat2->flags && mat1->m_pTexture->flags == mat2->m_pTexture->flags))
	{
		if (!texId)
		{
			g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_COLOROP, D3DTOP_MODULATE);
			g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_COLORARG2, D3DTA_DIFFUSE);
			g_dwPrevMaterialTexId = texId;
			if (mat1->flags & MATERIAL_HAS_ALPHAMIR)
			{
				if (g_bCanBlendCurrentAlpha && (gRenderFlags & 0x1000000))
				{
					g_dwPrevMaterialTexId = 1;
					g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
					g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				}
				// label 66
			}
			else if (mat1->flags & MATERIAL_HAS_TTX)
			{
				g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
				g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				// label 66
			}
			else
			{
				g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
				g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
				g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAARG2, D3DTA_DIFFUSE);
				// label 66
			}
		}
		else if (!(mat1->flags & MATERIAL_HAS_ITX))
		{
			if (mat1->flags & MATERIAL_HAS_BUMP)
			{
				g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_COLOROP, D3DTOP_BUMPENVMAP);
				g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_COLORARG2, D3DTA_CURRENT);
				g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
				g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAARG1, D3DTA_CURRENT);

				float param0 = field_2128 * 9.0;
				float param1 = cos(param0) * 0.03999999910593033;
				float param2 = sin(param0) * 0.03999999910593033;
				float param3 = sin(param0) * -0.03999999910593033;
				float param4 = cos(param0) * 0.03999999910593033;
				g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_BUMPENVMAT00, *(DWORD*)&param1);
				g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_BUMPENVMAT01, *(DWORD*)&param2);
				g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_BUMPENVMAT10, *(DWORD*)&param3);
				g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_BUMPENVMAT11, *(DWORD*)&param4);
				// label 66
			}
			else if (g_bCanBlendCurrentAlpha)
			{
				if (g_dwPrevMaterialTexId)
				{
					g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_COLOROP, D3DTOP_BLENDCURRENTALPHA);
				}
				else
				{
					if (mat1->flags & MATERIAL_HAS_REFLECTION && g_bCanBlendFactorAlpha || mat1->m_pTexture->m_dTextureFormat != 5)
						g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_COLOROP, D3DTOP_BLENDFACTORALPHA);
					else
						g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_COLOROP, D3DTOP_BLENDTEXTUREALPHA);
				}

				if (mat1->m_dwEnvId == 3)
					g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_COLOROP, D3DTOP_SELECTARG1);

				g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_COLORARG1, D3DTA_TEXTURE);
				g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_COLORARG2, D3DTA_CURRENT);

				if (g_bCanSelectArg1)
				{
					g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
					g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAARG2, g_dwPrevMaterialTexId == 0);
					// label 66
				}
				else
				{
					g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAOP, D3DTOP_SELECTARG1);
					g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAARG1, g_dwPrevMaterialTexId == 0);
				}
				// label 66
			}
		}
		else
		{
			g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_COLOROP, D3DTOP_MODULATE);
			g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_COLORARG1, D3DTA_TEXTURE);
			g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_COLORARG2, D3DTA_CURRENT);
			if (mat1->m_pTexture->m_dTextureFormat != 5)
			{
				g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAOP, D3DTOP_SELECTARG2);
				g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
			}
			g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAOP, D3DTOP_MODULATE);
			g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAARG1, D3DTA_TEXTURE);
			g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAARG2, D3DTA_CURRENT);
		}
		
	}

	if (mat1->m_pTexture->flags & 0x100 || mat1->flags & MATERIAL_NOTILE)
	{
		g_pDirect3DDevice->SetSamplerState(setTexId, D3DSAMP_ADDRESSU, D3DTADDRESS_CLAMP);
		g_pDirect3DDevice->SetSamplerState(setTexId, D3DSAMP_ADDRESSV, D3DTADDRESS_CLAMP);
	}
	else
	{
		g_pDirect3DDevice->SetSamplerState(setTexId, D3DSAMP_ADDRESSU, D3DTADDRESS_WRAP);
		g_pDirect3DDevice->SetSamplerState(setTexId, D3DSAMP_ADDRESSV, D3DTADDRESS_WRAP);
	}

	// label 66
	if (!(mat1->m_pTexture->flags & 8) || gRenderFlags & 0x4000000)
	{
		g_pDirect3DDevice->SetSamplerState(setTexId, D3DSAMP_MAGFILTER, D3DTEXF_LINEAR);
		g_pDirect3DDevice->SetSamplerState(setTexId, D3DSAMP_MINFILTER, D3DTEXF_LINEAR);
	}
	else
	{
		g_pDirect3DDevice->SetSamplerState(setTexId, D3DSAMP_MAGFILTER, D3DTEXF_POINT);
		g_pDirect3DDevice->SetSamplerState(setTexId, D3DSAMP_MINFILTER, D3DTEXF_POINT);
	}
	g_pDirect3DDevice->SetSamplerState(setTexId, D3DSAMP_MIPFILTER, D3DTEXF_LINEAR);

	setTexId++;
	Material *mat3 = 0;
	if (mat1->flags & MATERIAL_HAS_ALPHAMIR && !(gRenderFlags & 0x1000000))
	{
		g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_COLOROP, D3DTOP_DISABLE);
		g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	}
	else if (setTexId > 7)
	{
		return hasAlpha;
	}
	else if (mat2 && mat2->m_dwInternalId >= 0)
	{
		mat3 = &m_pMaterials[mat2->m_dwInternalId];
	}
	if (mat1->m_dwInternalId < 0 || mat1->flags & 0x20000)
	{
		g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_COLOROP, D3DTOP_DISABLE);
		g_pDirect3DDevice->SetTextureStageState(setTexId, D3DTSS_ALPHAOP, D3DTOP_DISABLE);
	}
	else
	{
		return hasAlpha | SetupMaterialsWithBlending(&m_pMaterials[mat1->m_dwInternalId], mat3, setTexId);
	}
	return hasAlpha;
}

bool D3DRenderData::IsSphereVisible(CVector4 *pos)
{
	return ((bool(__thiscall *)(D3DRenderData*, CVector4*))0x5B4F50)(this, pos);
}

void D3DRenderData::SetupWorldTransform(CMatrix *mat1, BOOL mult)
{
	return ((void(__thiscall *)(D3DRenderData*, CMatrix*, BOOL))0x5AE510)(this, mat1, mult);
}