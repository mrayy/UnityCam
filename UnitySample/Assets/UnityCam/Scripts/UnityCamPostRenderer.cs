using UnityEngine;
using System.Collections;

public class UnityCamPostRenderer : MonoBehaviour {
	UnityCam _ownerCamera;

	// Use this for initialization
	void Start () {
		_ownerCamera = gameObject.GetComponent<UnityCam> ();
	}


	// Update is called once per frame
	void Update () {
	
	}



	void OnRenderImage(RenderTexture source, RenderTexture destination) {
		_ownerCamera.RenderImage (source, destination);
	}
}
