using System;
using System.Collections;
using System.Collections.Generic;
using UnityEngine;

public class Controller : MonoBehaviour
{
    public Animator animator;

    private Vector3 oriScale = new Vector3();

    // Start is called before the first frame update
    void Start()
    {
        Debug.Assert(animator != null);

        oriScale = animator.transform.localScale;

        RandomTrigger();

        lark.LarkManager larkManager = lark.LarkManager.Instance;
        larkManager.DataChannel.onTaskStatus += OnTaskStatus;
        larkManager.DataChannel.onConnected += OnConnected;
        larkManager.DataChannel.onText += OnTextMessage;
        larkManager.DataChannel.onBinary += OnBinaryMessaeg;
        larkManager.DataChannel.onClose += OnClose;
        // 智能语音相关回调 
        larkManager.DataChannel.onAiVoiceURL += OnAiVoiceURL;
        larkManager.DataChannel.onAiVoiceStream += OnAiVoiceStream;

        // start connect
        lark.DataChannelNativeApi.ApiRestult restult = lark.LarkManager.Instance.StartConnect();

        Debug.Log("Connect Result restult=" + restult + " ;TaskId=" + lark.LarkManager.Instance.TaskId);

        // JsonCmd test = new JsonCmd(JsonCmd.CmdType.TOUCH_UP, 0, 0);

        // OnTextMessage(" { type: 1002, x: 1455.000000, y: 768.000000 } ");
        // OnTextMessage(" { \"type\": 1002, \"x\": 1455.000000, \"y\": 768.000000 } ");
        // OnTextMessage(test.ToJson());
    }


    // Update is called once per frame
    void Update()
    {
        if (Input.GetKeyUp(KeyCode.W)) {
            Walk();
        }
        if (Input.GetKeyUp(KeyCode.F)) {
            Fly();
        }
        if (Input.GetKeyUp(KeyCode.R)) {
            Run();
        }
        if (Input.GetKeyUp(KeyCode.I)) {
            Idel();
        }
    }

    void OnDestroy()
    {
    }
    #region trigger
    void RandomTrigger()
    {
        string[] triggers = { "walk", "fly", "run", "idel" };
        Trigger(triggers[UnityEngine.Random.Range(0, triggers.Length)]);
    }

    void Walk()
    {
        Trigger("walk");
    }

    void Fly()
    {
        Trigger("fly");
    }


    void Run()
    {
        Trigger("run");
    }

    void Idel()
    {
        Trigger("idel");
    }

    void Trigger(string trigger) {
        if (animator == null)
        {
            return;
        }
        animator.SetTrigger(trigger);
    }
    #endregion

    #region callbacks
    [System.Serializable]
    public class JsonCmd
    {
        public enum CmdType
        {
            TOUCH_DOWN = 1000,
            TOUCH_MOVE = 1001,
            TOUCH_UP   = 1002,
        }

        public static JsonCmd ParseJsonCmd(string res)
        {
            return JsonUtility.FromJson<JsonCmd>(res);
        }

        public CmdType type;
        public float x = 0;
        public float y = 0;

        public JsonCmd(CmdType type, float x, float y)
        {
            this.x = x;
            this.y = y;
        }

        public string ToJson()
        {
            return JsonUtility.ToJson(this);
        }
    }

    public void OnTextMessage(string msg)
    {
        Debug.Log("OnTextMessage " + msg);
        try
        {
            JsonCmd cmd = JsonCmd.ParseJsonCmd(msg);

            Debug.Log("cmd " + cmd + " " + cmd.ToJson());

            if (cmd.type == JsonCmd.CmdType.TOUCH_UP)
            {
                RandomTrigger();
            }
        } catch (Exception)
        {
            Debug.Log("unkown json message " + msg);
        }
    }

    public void OnTaskStatus(bool status, string taskId)
    {
        Debug.Log("on task status change " + status + " " + taskId);
    }
    public void OnConnected()
    {
        Debug.Log("OnConnected ");
    }
    public void OnBinaryMessaeg(byte[] binary)
    {
        Debug.Log("OnBinaryMessaeg " + BitConverter.ToString(binary));
    }
    public void OnClose(lark.DataChannelNativeApi.ErrorCode code)
    {
        Debug.Log("OnClose " + code);
    }
    public void OnAiVoiceURL(lark.DataChannelNativeApi.AiVoiceURL aiVoiceURL)
    {
        Debug.Log("OnAiVoiceURL " + aiVoiceURL.nlg + " url " + aiVoiceURL.online_url);
    }

    public void OnAiVoiceStream(lark.DataChannelNativeApi.AiVoiceStream aiVoiceStream)
    {
        Debug.Log("OnAiVoiceURL " + aiVoiceStream.nlg);
    }
    #endregion
}
