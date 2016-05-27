//
// UnityWebcam
//
// MHD Yamen Saraiji
// yamen@myamens.com
// 
// TextureWrapper
// Convert almost any type of textures into Texture2D
// Helpful to access texture pixels


using UnityEngine;
using System.Collections;

public class TextureWrapper 
{


	Texture2D _wrappedTexture;

	public Texture2D WrappedTexture
	{
		get { return _wrappedTexture; } 
	}


	public Texture2D ConvertTexture(Texture src)
	{
		if (src == null)
			return null;
		if (src as Texture2D != null) {
			_wrappedTexture = src as Texture2D;
			return _wrappedTexture;
		}
		if (_wrappedTexture == null ||
			_wrappedTexture.width != src.width ||
			_wrappedTexture.height != src.height) {
			_wrappedTexture = new Texture2D (src.width, src.height, TextureFormat.ARGB32, false);
		}

		if (src as WebCamTexture != null) {
			_wrappedTexture.SetPixels((src as WebCamTexture).GetPixels ());
			_wrappedTexture.Apply ();
			return _wrappedTexture;
		}

		if (src as RenderTexture != null) {
			RenderTexture.active = src as RenderTexture;
			_wrappedTexture.ReadPixels (new Rect (0, 0, src.width, src.height), 0, 0);
			_wrappedTexture.Apply ();
			RenderTexture.active = null;
			return _wrappedTexture;
		}

		return null;
	}
}
