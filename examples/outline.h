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

 // Обращение к StartKinectST при запуске приложения.
    HRESULT MyApplication::StartKinectST()
    {
      m_hNextSkeletonEvent = NULL;

      // Инициализация m_pNuiSensor
      HRESULT hr = FindKinectSensor();
      if (SUCCEEDED(hr))
      {
        // Инициализация Kinect и указать, что используем скелет
        hr = m_pNuiSensor->NuiInitialize(NUI_INITIALIZE_FLAG_USES_SKELETON);
        if (SUCCEEDED(hr))
        {
          // Создать событие, которое будет сигнализировать, когда скелет данные доступны
          m_hNextSkeletonEvent = CreateEventW(NULL, TRUE, FALSE, NULL);

          // Запуск потока для получения данных
          hr = m_pNuiSensor->NuiSkeletonTrackingEnable(m_hNextSkeletonEvent, 0);
        }
      }
      return hr;
    }

 // Обращение к UpdateKinectST при каждой итерации цикла обновления
    void MyApplication::UpdateKinectST()
    {
      // Ожидание 0ms, для проверки времени обработки скилета
      if ( WAIT_OBJECT_0 == WaitForSingleObject(m_hNextSkeletonEvent, 0) )
      {
        NUI_SKELETON_FRAME skeletonFrame = {0};

        // Получение кадра обработанного скилета
        if (SUCCEEDED(m_pNuiSensor->NuiSkeletonGetNextFrame(0, &skeletonFrame)))
        {
          // Определение кадра
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
    // Колличество участников
  }
			//Получение доступа к совместной информацие и создание скелета
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
      // Render голова и плечи
      DrawBone(skeleton, NUI_SKELETON_POSITION_HEAD, NUI_SKELETON_POSITION_SHOULDER_CENTER);
      DrawBone(skeleton, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_LEFT);
      DrawBone(skeleton, NUI_SKELETON_POSITION_SHOULDER_CENTER, NUI_SKELETON_POSITION_SHOULDER_RIGHT);

      // Render левая рука
      DrawBone(skeleton, NUI_SKELETON_POSITION_SHOULDER_LEFT, NUI_SKELETON_POSITION_ELBOW_LEFT);
      DrawBone(skeleton, NUI_SKELETON_POSITION_ELBOW_LEFT, NUI_SKELETON_POSITION_WRIST_LEFT);
      DrawBone(skeleton, NUI_SKELETON_POSITION_WRIST_LEFT, NUI_SKELETON_POSITION_HAND_LEFT);

      // Render правая рука
      DrawBone(skeleton, NUI_SKELETON_POSITION_SHOULDER_RIGHT, NUI_SKELETON_POSITION_ELBOW_RIGHT);
      DrawBone(skeleton, NUI_SKELETON_POSITION_ELBOW_RIGHT, NUI_SKELETON_POSITION_WRIST_RIGHT);
      DrawBone(skeleton, NUI_SKELETON_POSITION_WRIST_RIGHT, NUI_SKELETON_POSITION_HAND_RIGHT);

      // Render других частей...
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
          return; // пропустить выполнение, один из суставов не отслеживается
        }

        const Vector4& jointFromPosition = skeleton.SkeletonPositions[jointFrom];
        const Vector4& jointToPosition = skeleton.SkeletonPositions[jointTo];

        // Прекращение рисования если отслеживание возобновилось
        if (jointFromState == NUI_SKELETON_POSITION_INFERRED || jointToState == NUI_SKELETON_POSITION_INFERRED)
        {
          DrawNonTrackedBoneLine(jointFromPosition, jointToPosition); // Draw thin lines if either one of the joints is inferred
        }

        // Вывод костей при условие, что оба сустава не отслеживаются
        if (jointFromState == NUI_SKELETON_POSITION_TRACKED && jointToState == NUI_SKELETON_POSITION_TRACKED)
        {
          DrawTrackedBoneLine(jointFromPosition, jointToPosition); // При отслеживании линии 2 суставов
        }
      }
	 
			//доступ к обрезпнию края
	   void MyApplication::RenderClippedEdges(const NUI_SKELETON_DATA & skeleton)
    {
      if (skeleton.dwQualityFlags & NUI_SKELETON_QUALITY_CLIPPED_BOTTOM)
      {
        DrawClippedEdges(NUI_SKELETON_QUALITY_CLIPPED_BOTTOM); // Выделение красным при достижении определенной зоны
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

    float closestDistance = 10000.0f; // Начинаем с больного расстояния
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
        m_pNuiSensor->SetTrackedSkeletons(closestIDs); // Определение скилета
      }
   }
   
