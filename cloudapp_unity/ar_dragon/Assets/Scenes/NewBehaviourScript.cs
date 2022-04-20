using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class NewBehaviourScript : MonoBehaviour
{
    const int ROW_MAX = 5;
    const int LINE_MAX = 5;
    const int PLANE_MAX = ROW_MAX * LINE_MAX;

    public Transform objectContainer;
    public GameObject cubeBase;

    List<GameObject> cubeList = new List<GameObject>();

    // Start is called before the first frame update
    void Start()
    {
        for (int i = 0; i < 100; i++) {
            CreateCube();
        }
    }

    // Update is called once per frame
    void Update()
    {
        
    }

    public void CreateCube()
    {
        if (objectContainer == null)
        {
            return;
        }
        if (cubeBase == null)
        {
            return;
        }
        int index = cubeList.Count + 1;
        int d = index / PLANE_MAX;
        int planeIndex = index % PLANE_MAX;
        int r = planeIndex / LINE_MAX;
        int l = planeIndex % LINE_MAX;
        GameObject cube = Instantiate(cubeBase, objectContainer);
        cube.transform.position = new Vector3(2 * l - 5, 2 * r - 5, d * 2);
        cubeList.Add(cube);
    }
}
