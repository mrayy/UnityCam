//Copyright (c) 2016  MHD Yamen Saraiji



using UnityEngine;
using System.Runtime.InteropServices;

[RequireComponent(typeof(Camera))]
public class UnityCam : MonoBehaviour {

	internal const string DllName="UnityWebcam";

	[DllImport(DllName,CallingConvention=CallingConvention.Cdecl)]
	extern static private System.IntPtr CreateTextureWrapper();

	[DllImport(DllName,CallingConvention=CallingConvention.Cdecl)]
	extern static private void DeleteTextureWrapper(System.IntPtr w);

	[DllImport(DllName,CallingConvention=CallingConvention.Cdecl)]
	extern static private bool SendTexture(System.IntPtr w,System.IntPtr textureID);


	System.IntPtr _instance;

	public Texture ResultTexture;

	public bool Flip=false;
	public bool BlitLocaly=true;

	TextureWrapper _wrapper;

	OffscreenProcessor _BlitterProcessor;


	void Start() {
		//Init UnityWebCamera plugin
		_instance = CreateTextureWrapper ();

		_BlitterProcessor = new OffscreenProcessor ();
		_BlitterProcessor.ShaderName = "UnityCam/Image/Blitter";

		_wrapper = new TextureWrapper ();

		//Add Post Renderer invoker, it will handle the rest
		gameObject.AddComponent<UnityCamPostRenderer> ();
	}

	//will be invoked by HUGVRPostRenderer
	public void RenderImage(RenderTexture source, RenderTexture destination) {
		Texture tex = source;

		if(Flip)
			tex=_BlitterProcessor.ProcessTexture(tex,0);
		else 
			tex=_BlitterProcessor.ProcessTexture(tex,1);
		
		_wrapper.ConvertTexture (tex);
		tex = _wrapper.WrappedTexture;
		ResultTexture = tex;

		//Send the rendered image to the plugin 
		SendTexture (_instance,tex.GetNativeTexturePtr());

		if(BlitLocaly)
			Graphics.Blit (source, destination);
	}

	void OnDestroy() {
		for (int i = 0; i < transform.childCount; i++) {
			GameObject obj = transform.GetChild(i).gameObject;
			DestroyImmediate(obj);
		}
	}

	class RenderEvent : MonoBehaviour {
		public Material material;

		void OnRenderImage(RenderTexture source, RenderTexture destination) {
			if (material == null) {
				Graphics.Blit(source, destination);
			}
			Graphics.Blit(source, destination, material);
		}

		void OnDestroy() {
			DestroyImmediate(material);
		}
	}

}