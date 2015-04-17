			//Включение режима отслеживания скилета 
/*
public:
HRESULT NuiSkeletonTrackingEnable(
         HANDLE hNextFrameEvent,
         DWORD dwFlags
)
----------------------------------------
public:
HRESULT NuiSkeletonGetNextFrame(
         DWORD dwMillisecondsToWait,
         NUI_SKELETON_FRAME *pSkeletonFrame
)
*/

 // Call StartKinectST once at application start.
    HRESULT MyApplication::StartKinectST()
    {
      m_hNextSkeletonEvent = NULL;

      // Initialize m_pNuiSensor
      HRESULT hr = FindKinectSensor();
      if (SUCCEEDED(hr))
      {
        // Initialize the Kinect and specify that we'll be using skeleton
        hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON);
        if (SUCCEEDED(hr))
        {
          // Create an event that will be signaled when skeleton data is available
          m_hNextSkeletonEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

          // Open a skeleton stream to receive skeleton data
          hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, 0);
        }
      }
      return hr;
    }

 // Call UpdateKinectST on each iteration of the application's update loop.
    void MyApplication::UpdateKinectST()
    {
      // Wait for 0ms, just quickly test if it is time to process a skeleton
      if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hNextSkeletonEvent, 0) )
      {
        NUI_SKELETON_FRAME skeletonFrame = {0};

        // Get the skeleton frame that is ready
        if (SUCCEEDED(m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame)))
        {
          // Process the skeleton frame
          SkeletonFrameReady(&skeletonFrame);
        }
      }
    }

			//Получение доступа к информации Skeletal Tracking
/*
public:
HRESULT NuiSkeletonGetNextFrame(
         DWORD dwMillisecondsToWait,
         NUI_SKELETON_FRAME *pSkeletonFrame
)
-----------------------------------------
typedef struct _NUI_SKELETON_FRAME {
    LARGE_INTEGER liTimeStamp;
    DWORD dwFrameNumber;
    DWORD dwFlags;
    Vector4 vFloorClipPlane;
    Vector4 vNormalToGravity;
    NUI_SKELETON_DATA SkeletonData[NUI_SKELETON_COUNT];
} NUI_SKELETON_FRAME;
*/
void MyApplication::SkeletonFrameReady(NUI_SKELETON_FRAME* pSkeletonFrame)
  {
    // Access members of pSkeletonFrame...
  }
			//Получение доступа к совместной информацие
			//и создание скелета
/*
typedef struct _NUI_SKELETON_DATA {
    NUI_SKELETON_TRACKING_STATE eTrackingState;
    DWORD dwTrackingID;
    DWORD dwEnrollmentIndex;
    DWORD dwUserIndex;
    Vector4 Position;
    Vector4 SkeletonPositions[20];
    NUI_SKELETON_POSITION_TRACKING_STATE eSkeletonPositionTrackingState[20];
    DWORD dwQualityFlags;
} NUI_SKELETON_DATA;
*/
  void MyApplication::DrawSkeleton(const NUI_SKELETON_DATA & skeleton)
    {
      // Render Head and Shoulders
      DrawBone(skeleton, NUI_SKELETON_POSITION_HEAD, NUI_SKELETON_POSITION_SHOULDER_CENTER);
      DrawBone(skeleton, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_LEFT);
      DrawBone(skeleton, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_RIGHT);

      // Render Left Arm
      DrawBone(skeleton, NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT);
      DrawBone(skeleton, NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT);
      DrawBone(skeleton, NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT);

      // Render Right Arm
      DrawBone(skeleton, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT);
      DrawBone(skeleton, NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT);
      DrawBone(skeleton, NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT);

      // Render other bones...
    }

    void MyApplication::DrawBone(
      const NUI_SKELETON_DATA & skeleton,
      NUI_SKELETON_POSITION_INDEX jointFrom,
      NUI_SKELETON_POSITION_INDEX jointTo)
      {
	 
        NUI_SKELETON_POSITION_TRACKING_STATE jointFromState = skeleton.eSkeletonPositionTrackingState[jointFrom];
        NUI_SKELETON_POSITION_TRACKING_STATE jointToState = skeleton.eSkeletonPositionTrackingState[jointTo];

        if (jointFromState == NUI_SKELETON_POSITION_NOT_TRACKED || jointToState == NUI_SKELETON_POSITION_NOT_TRACKED)
        {
          return; // nothing to draw, one of the joints is not tracked
        }

        const Vector4& jointFromPosition = skeleton.SkeletonPositions[jointFrom];
        const Vector4& jointToPosition = skeleton.SkeletonPositions[jointTo];

        // Don't draw if both points are inferred
        if (jointFromState == NUI_SKELETON_POSITION_INFERRED || jointToState == NUI_SKELETON_POSITION_INFERRED)
        {
          DrawNonTrackedBoneLine(jointFromPosition, jointToPosition); // Draw thin lines if either one of the joints is inferred
        }

        // We assume all drawn bones are inferred unless BOTH joints are tracked
        if (jointFromState == NUI_SKELETON_POSITION_TRACKED && jointToState == NUI_SKELETON_POSITION_TRACKED)
        {
          DrawTrackedBoneLine(jointFromPosition, jointToPosition); // Draw bold lines if the joints are both tracked
        }
      }
	 
			//доступ к обрезпнию края
	   void MyApplication::RenderClippedEdges(const NUI_SKELETON_DATA & skeleton)
    {
      if (skeleton.dwQualityFlags & NUI_SKELETON_QUALITY_CLIPPED_BOTTOM)
      {
        DrawClippedEdges(NUI_SKELETON_QUALITY_CLIPPED_BOTTOM); // Make the border red to show the user is reaching the border
      }

      if (skeleton.dwQualityFlags & NUI_SKELETON_QUALITY_CLIPPED_TOP)
      {
        DrawClippedEdges(NUI_SKELETON_QUALITY_CLIPPED_TOP);
      }

      if (skeleton.dwQualityFlags & NUI_SKELETON_QUALITY_CLIPPED_LEFT)
      {
        DrawClippedEdges(NUI_SKELETON_QUALITY_CLIPPED_LEFT);
      }

      if (skeleton.dwQualityFlags & NUI_SKELETON_QUALITY_CLIPPED_RIGHT)
      {
        DrawClippedEdges(NUI_SKELETON_QUALITY_CLIPPED_RIGHT);
      }
    }
	
			//Вобор пользователя для отслеживания 
/*public:
HRESULT NuiSkeletonTrackingEnable(
         HANDLE hNextFrameEvent,
         DWORD dwFlags
)
*/
void MyApplication::TrackClosestSkeleton(NUI_SKELETON_FRAME* pSkeletonFrame)
  {
  
    m_pNuiSensor->NuiSkeletonTrackingEnable(
    m_hNextSkeletonEvent,
    NUI_SKELETON_TRACKING_FLAG_TITLE_SETS_TRACKED_SKELETONS);

    float closestDistance = 10000.0f; // Start with a far enough distance
    DWORD closestIDs[NUI_SKELETON_MAX_TRACKED_COUNT] = {0, 0};

    for (int i = 0; i < NUI_SKELETON_COUNT; i++)
    {
      const NUI_SKELETON_DATA & skeleton = pSkeletonFrame->SkeletonData[i];
      if (skeleton.eTrackingState != NUI_SKELETON_NOT_TRACKED)
      {
        if (skeleton.Position.z < closestDistance)
        {
          closestIDs[0] = skeleton.dwTrackingID;
          closestDistance = skeleton.Position.z;
        }
      }
 
      if (closestIDs[0] > 0)
      {
        m_pNuiSensor->SetTrackedSkeletons(closestIDs); // Track this skeleton
      }
   }
   
