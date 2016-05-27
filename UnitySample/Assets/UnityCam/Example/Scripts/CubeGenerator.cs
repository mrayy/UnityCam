using UnityEngine;
using System.Collections;

public class CubeGenerator : MonoBehaviour {

	public int Count=10;

	// Use this for initialization
	void Start () {
		for(int i=0;i<Count;++i)
			for(int j=0;j<Count;++j)
				for(int k=0;k<Count;++k)
				{
					GameObject o = GameObject.CreatePrimitive (PrimitiveType.Cube);

					o.transform.parent = this.transform;
					o.transform.localPosition = new Vector3 (5 * (i - Count / 2), 5 * (j - Count / 2), 5 * (k- Count / 2));

					o.AddComponent<ObjectMotion> ();
				}
	}
	
	// Update is called once per frame
	void Update () {
	
	}
}
