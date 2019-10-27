using System;
using UnityEngine;

namespace UnityStandardAssets.ImageEffects
{
    [RequireComponent(typeof (Camera))]
    [AddComponentMenu("")]
    public class ImageEffectBase : MonoBehaviour
    {
        /// Provides a shader property that is set in the inspector
        /// and a material instantiated from the shader
        public Shader shader;

        private Material m_Material;


        protected virtual void Start()
        {
#if UNITY_5_5_1
			// Disable if we don't support image effects
			if (!SystemInfo.supportsImageEffects)
            {
                enabled = false;
                return;
            }
#elif UNITY_2019
#endif
			// Disable the image effect if the shader can't
			// run on the users graphics card
			if (!shader || !shader.isSupported)
                enabled = false;
		}


		protected Material material
        {
            get
            {
                if (m_Material == null)
                {
                    m_Material = new Material(shader);
                    m_Material.hideFlags = HideFlags.HideAndDontSave;
                }
                return m_Material;
            }
        }


        protected virtual void OnDisable()
        {
            if (m_Material)
            {
                DestroyImmediate(m_Material);
            }
        }
    }
}
