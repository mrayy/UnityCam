using UnityEngine;
using System.Collections;

public class ObjectMotion : MonoBehaviour {

	float _time;
	Vector3 _Speed;
	// Use this for initialization
	void Start () {
		_Speed = Random.insideUnitSphere * 50;
	}


	// Update is called once per frame
	void Update () {

		transform.localRotation = Quaternion.Euler (_Speed * _time);
		_time += Time.deltaTime;
	
	}
}
