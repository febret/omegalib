/**************************************************************************************************
 * THE OMEGA LIB PROJECT
 *-------------------------------------------------------------------------------------------------
 * Copyright 2010-2013		Electronic Visualization Laboratory, University of Illinois at Chicago
 * Authors:										
 *  Alessandro Febretti		febret@gmail.com
 *-------------------------------------------------------------------------------------------------
 * Copyright (c) 2010-2013, Electronic Visualization Laboratory, University of Illinois at Chicago
 * All rights reserved.
 * Redistribution and use in source and binary forms, with or without modification, are permitted 
 * provided that the following conditions are met:
 * 
 * Redistributions of source code must retain the above copyright notice, this list of conditions 
 * and the following disclaimer. Redistributions in binary form must reproduce the above copyright 
 * notice, this list of conditions and the following disclaimer in the documentation and/or other 
 * materials provided with the distribution. 
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR 
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO THE IMPLIED WARRANTIES OF MERCHANTABILITY AND 
 * FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR 
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL 
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE  GOODS OR SERVICES; LOSS OF 
 * USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, 
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN 
 * ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *************************************************************************************************/
#include "cyclops/Material.h"
#include "cyclops/SceneManager.h"

#include <osg/PolygonMode>
#include<osg/BlendFunc>

using namespace cyclops;

///////////////////////////////////////////////////////////////////////////////////////////////////
Material::Material(osg::StateSet* ss, SceneManager* sm): Uniforms(ss), myStateSet(ss), myTransparent(false), mySceneManager(sm)
{
	reset();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Material::setColor(const Color& diffuse, const Color& emissive)
{
	if(myMaterial == NULL)
	{
		myMaterial = new osg::Material();
		myStateSet->setAttributeAndModes(myMaterial, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	}
	myMaterial->setDiffuse(osg::Material::FRONT_AND_BACK, COLOR_TO_OSG(diffuse));
	myMaterial->setEmission(osg::Material::FRONT_AND_BACK, COLOR_TO_OSG(emissive));
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Material::setShininess(float value)
{
	myMaterial->setShininess(osg::Material::FRONT_AND_BACK, value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Material::setGloss(float value)
{
}

///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
void Material::setTransparent(bool value)
{
	myTransparent = value;
	if(myTransparent)
	{
		myStateSet->setRenderingHint(osg::StateSet::TRANSPARENT_BIN);
		myStateSet->setMode(GL_BLEND, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	}
	else
	{
		myStateSet->setRenderingHint(osg::StateSet::OPAQUE_BIN);
		myStateSet->setMode(GL_BLEND, osg::StateAttribute::OFF| osg::StateAttribute::OVERRIDE);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Material::setAlpha(float value)
{
	myAlpha->setFloat(value);
}

///////////////////////////////////////////////////////////////////////////////////////////////////
float Material::getAlpha()
{
	return myAlpha->getFloat();
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Material::setLightingEnabled(bool value)
{
	if(value)
	{
		myStateSet->setMode(GL_LIGHTING, osg::StateAttribute::ON | osg::StateAttribute::OVERRIDE);
	}
	else
	{
		myStateSet->setMode(GL_LIGHTING, osg::StateAttribute::OFF | osg::StateAttribute::OVERRIDE);
	}
}

///////////////////////////////////////////////////////////////////////////////////////////////////
void Material::reset()
{
	removeAllUniforms();
	myStateSet->clear();
	myMaterial = NULL;
	myAlpha = addUniform("unif_Alpha", Uniform::Float);
	myAlpha->setFloat(1.0f);
	myStateSet->setNestRenderBins(false);

	setLightingEnabled(true);
}