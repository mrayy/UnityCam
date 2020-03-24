//
// UnityWebcam
//
// MHD Yamen Saraiji
// yamen@myamens.com
// 
// OffscreenProcessor
// Used to apply a shader on a texture 


using UnityEngine;
using System.Collections;

public class OffscreenProcessor  {

	Material _ProcessingMaterial;
	public RenderTexture ResultTexture{
		get{
			return _RenderTexture;
		}
	}
	public Material ProcessingMaterial {
		get{ return _ProcessingMaterial; }
	}
	RenderTexture _RenderTexture;

	public RenderTextureFormat TargetFormat;

	public string ShaderName
	{
		set{
			ProcessingShader=Shader.Find(value);
		}
	}
	public Shader ProcessingShader
	{
		set{
			ProcessingMaterial.shader=value;
		}
		get{
			return ProcessingMaterial.shader;
		}
	}

	public OffscreenProcessor(string shader= "Diffuse")
	{
        Shader s = Shader.Find(shader);
        _ProcessingMaterial = new Material (s);
		TargetFormat = RenderTextureFormat.Default;
	}
	void _Setup(Texture InputTexture,int downSample)
	{
		int width = InputTexture.width/(downSample+1);
		int height = InputTexture.height/(downSample+1);
		if ( (InputTexture as Texture2D !=null) && ((Texture2D)InputTexture).format == TextureFormat.Alpha8)
			height =(int)( height / 1.5f);
		if (_RenderTexture == null) {
			_RenderTexture = new RenderTexture (width, height,16, TargetFormat);
		} else if (	_RenderTexture.width != width || 
		           _RenderTexture.height != height) 
		{
			_RenderTexture = new RenderTexture (width, height,16, TargetFormat);
		}
	}
	public Texture ProcessTexture(Texture InputTexture,int pass=0,int downSample=0)
	{
		if (InputTexture==null || InputTexture.width == 0 || InputTexture.height == 0)
			return InputTexture;
		_Setup (InputTexture,downSample);
		ProcessingMaterial.mainTexture = InputTexture;
		Graphics.Blit (InputTexture,_RenderTexture, ProcessingMaterial,pass);
		return _RenderTexture;

	}

}
