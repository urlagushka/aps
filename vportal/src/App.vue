<template>
  <div id="app">
    <div class="container">
      <!-- Управление -->
      <div class="column management">
        <h2>Управление</h2>

        <div class="settings">
          <div class="setting">
            <label for="converter-count">Количество конверторов:</label>
            <input
              type="number"
              id="converter-count"
              v-model.number="cvv_size"
              :disabled="is_running"
              min="1"
            />
          </div>

          <div class="setting">
            <label for="queue-size">Количество мест в очереди:</label>
            <input
              type="number"
              id="queue-size"
              v-model.number="pqm_size"
              :disabled="is_running"
              min="1"
            />
          </div>

          <div class="setting">
            <label for="client-interval">Интервал между клиентами (мс):</label>
            <input
              type="number"
              id="client-interval"
              v-model.number="cl_interval"
              :disabled="is_running"
              min="0"
            />
          </div>

          <div class="setting">
            <label>Размер файла (МБ):</label>
            <div class="file-size-range">
              <input
                type="number"
                v-model.number="filesize_min"
                :disabled="is_running"
                min="1"
                :max="filesize_max"
                placeholder="От"
              />
              <span>до</span>
              <input
                type="number"
                v-model.number="filesize_max"
                :disabled="is_running"
                :min="filesize_min"
                max="1000"
                placeholder="До"
              />
            </div>
          </div>
        </div>

        <div class="status">
          <p>Статус процесса: 
            <span :class="['status-text', is_running ? 'running' : 'stopped']">
              {{ is_running ? 'Запущен' : 'Остановлен' }}
            </span>
          </p>
        </div>

        <!-- Кнопки Старт/Стоп -->
        <div class="controls">
          <button @click="start" :disabled="is_running">Старт</button>
          <button @click="stop" :disabled="!is_running">Стоп</button>
        </div>
      </div>

      <!-- Аналитика по конверторам -->
      <div class="column analytics">
        <h2>Аналитика по конверторам</h2>
        <div class="converter-list">
          <div
            v-for="converter in converters"
            :key="converter.id"
            class="converter-card"
          >
            <p><strong>Конвертор #{{ converter.id }}</strong></p>
            <p>Файл: {{ converter.filename || 'Нет файла' }}</p>
            <p>Размер: {{ converter.filesize || '—' }}</p>
            <p>Исходный формат: {{ converter.source_fmt || '—' }}</p>
            <p>Целевой формат: {{ converter.target_fmt || '—' }}</p>
            <p>Статус: {{ converter.status }}</p>
          </div>
        </div>
      </div>

      <!-- Аналитика по клиентам -->
      <div class="column analytics">
        <h2>Аналитика по клиентам</h2>
        <div class="client-list" ref="client_list">
          <div
            v-for="client in clients"
            :key="client.id"
            class="client-card"
          >
            <p><strong>Клиент #{{ client.id }}</strong></p>
            <p>Файл: {{ client.filename }}</p>
            <p>Размер: {{ client.filesize }} МБ</p>
            <p>Исходный формат: {{ client.source_fmt }}</p>
            <p>Целевой формат: {{ client.target_fmt }}</p>
            <p>Премиум: {{client.premium}}</p>
            <p>Статус: {{ client.status }}</p>
          </div>
        </div>
      </div>
    </div>

    <!-- Логи -->
    <div class="logs">
      <h2>Логи</h2>
      <div class="log-list" ref="log_list">
        <div
          v-for="(log, index) in logs"
          :key="index"
          class="log-item"
          :class="log.status"
        >
          <span class="log-time">{{ log.time }}</span>
          <span class="log-status">{{ log.status }}</span>
          <span class="log-message">{{ log.message }}</span>
        </div>
      </div>
    </div>
  </div>
</template>

<script>
import axios from 'axios';

export default {
  name: 'App',
  data() {
    return {
      is_running: false,
      cvv_size: 5,
      pqm_size: 10,
      cl_interval: 1000,
      filesize_min: 1,
      filesize_max: 100,
      converters: [],
      clients: [],
      logs: [],
      cl_gen_interval: null,
      cl_counter: 1,
    };
  },

  methods: {
    scrollLogsToBottom() {
      this.$nextTick(() => {
        const log_list = this.$refs.log_list;
        if (log_list) {
          log_list.scrollTop = log_list.scrollHeight;
        }
      });
    },
    scrollLogsToBottom1() {
      this.$nextTick(() => {
        const client_list = this.$refs.client_list;
        if (client_list) {
          client_list.scrollTop = client_list.scrollHeight;
        }
      });
    },
    get_curr_time() {
      const now = new Date();
      return now.toLocaleTimeString();
    },

    async start() {
      try {
        await axios.post('http://localhost:8085/start', {
          pqm_size: this.pqm_size,
        });
        await axios.post('http://localhost:8086/start', {
          cvv_size: this.cvv_size,
        });

        this.is_running = true;
        this.start_cl_generation();

        this.logs.push({
          time: this.get_curr_time(),
          status: 'успех',
          message: 'Процесс запущен',
        });
      } catch (error) {
        console.error('Ошибка при запуске процесса:', error);
        this.logs.push({
          time: this.get_curr_time(),
          status: 'ошибка',
          message: 'Ошибка при запуске процесса',
        });
      }
      for (let i = 1; i <= this.cvv_size; ++i)
      {
        const converter = {
          id: i,
          filename: "-",
          filesize: "-",
          source_fmt: "-",
          target_fmt: "-",
          status: 'свободен',
        };
        this.converters.push(converter);
      }
    },

    async stop() {
      try {
        await axios.post('http://localhost:8085/stop', {});
        await axios.post('http://localhost:8086/stop', {});

        this.is_running = false;
        this.stop_cl_generation();

        this.generateSummary();

        this.clients = [];
        this.converters = [];
        this.cl_counter = 1;

        this.logs.push({
          time: this.get_curr_time(),
          status: 'успех',
          message: 'Процесс остановлен',
        });
      } catch (error) {
        console.error('Ошибка при остановке процесса:', error);
        this.logs.push({
          time: this.get_curr_time(),
          status: 'ошибка',
          message: 'Ошибка при остановке процесса',
        });
      }
    },

    generateSummary() {
            // Формируем заголовки CSV
            const headers = [
        'ID клиента',
        'Файл клиента',
        'Размер файла клиента',
        'Исходный формат клиента',
        'Целевой формат клиента',
        'Премиум',
        'Статус клиента'
      ];

      // Формируем строки CSV
      const rows = [];
        this.clients.forEach(client => {
          const row = [
            client.id,
            client.filename,
            client.filesize,
            client.source_fmt,
            client.target_fmt,
            client.premium,
            client.status
          ];
          rows.push(row.join(','));
      });

      // Объединяем заголовки и строки
      const csvContent = [headers.join(','), ...rows].join('\n');

      // Создаем Blob и ссылку для скачивания
      const blob = new Blob([csvContent], { type: 'text/csv;charset=utf-8;' });
      const link = document.createElement('a');
      link.href = URL.createObjectURL(blob);
      link.download = 'summary.csv';
      link.click();
      URL.revokeObjectURL(link.href);
    },

    start_cl_generation() {
      this.cl_gen_interval = setInterval(() => {
        this.generate_client();
      }, this.cl_interval);
    },

    stop_cl_generation() {
      if (this.cl_gen_interval) {
        clearInterval(this.cl_gen_interval);
        this.cl_gen_interval = null;
      }
    },

    async generate_client() {
      const ex = ["MP4", "AVI", "AV1", "MOV", "M4V", "MPG", "WMV"].sort(() => Math.random() - 0.5);
      const client = {
        id: this.cl_counter,
        filename: `file_${this.cl_counter}.mp4`,
        filesize: Math.floor(Math.random() * (this.filesize_max - this.filesize_min + 1)) + this.filesize_min,
        source_fmt: ex[0],
        target_fmt: ex[1],
        status: 'ожидание',
        premium: Math.random() < 0.5 ? 0 : 1,
      };
      // let l1;
      // let l2;
      try {
        
        await axios.post('http://localhost:8085/add', client)
          .then((response) => {
            const tmp = JSON.parse(response.data);
            if (tmp.status == 0)
            {
              this.clients.push(client);
              this.cl_counter++;
              this.logs.push({
                time: this.get_curr_time(),
                status: 'успех',
                message: `Клиент #${client.id} добавлен в очередь`,
              });
            }
            else if (tmp.status == 1)
            {
              // l1 = tmp.item.id;
              // l2 = this.clients[tmp.item.id - 1].id;
              this.clients[tmp.item.id - 1].status = tmp.item.status;
              this.clients.push(client);
              this.cl_counter++;
              this.logs.push({
                time: this.get_curr_time(),
                status: 'предупреждение',
                message: `Клиент #${client.id} добавлен в очередь, клиент #${tmp.item.id} выброшен`,
              });
            }
            else
            {
              this.logs.push({
                time: this.get_curr_time(),
                status: 'ошибка',
                message: `Ошибка при добавлении клиента: ${tmp.error}`,
              });
            }
          });
      } catch (error) {
        // alert(`${l1} : ${l2} : ${this.clients.length}`);
        this.logs.push({
          time: this.get_curr_time(),
          status: 'ошибка',
          message: `Ошибка при добавлении клиента: ${error}`,
        });
      }
    },
  },
  mounted() {
    setInterval(() => {
      try {
        if (!this.is_running)
        {
          return;
        }
        axios.post('http://localhost:8085/update_cls', {})
          .then((response) => {
            const tmp = JSON.parse(response.data);
            for (const item of tmp.items)
            {
              for (let client of this.clients)
              {
                if (client.id == item.id)
                {
                  client.status = item.status;
                    this.logs.push({
                    time: this.get_curr_time(),
                    status: 'успех',
                    message: `Клиент #${client.id} статус установлен на ${client.status}`,
                  });
                }
              }
            }
          });
      } catch (error) {
        this.logs.push({
          time: this.get_curr_time(),
          status: 'ошибка',
          message: 'Ошибка при обновлении клиента',
        });
      }
    }, 100);

    setInterval(() => {
    try {
        if (!this.is_running)
        {
          return;
        }
        axios.post('http://localhost:8086/update_cls', {})
          .then((response) => {
            const tmp = response.data;
            for (const item of tmp.items)
            {
              for (let client of this.clients)
              {
                if (client.id == item.id)
                {
                  client.status = item.status;
                    this.logs.push({
                    time: this.get_curr_time(),
                    status: 'успех',
                    message: `Клиент #${client.id} статус установлен на ${client.status}`,
                  });
                }
              }
            }
          });
      } catch (error) {
        this.logs.push({
          time: this.get_curr_time(),
          status: 'ошибка',
          message: 'Ошибка при обновлении клиента',
        });
      }
    }, 100);

    setInterval(() => {
      try {
        if (!this.is_running)
        {
          return;
        }
        axios.post('http://localhost:8086/update_cvs', {})
          .then((response) => {
            // alert(JSON.stringify(response.data, null, 2));
            const tmp = response.data;
            for (const item of tmp.items)
            {
              for (let i = 0; i < this.converters.length; i++)
              {
                if (this.converters[i].id == item.id)
                {
                  this.converters[i] = item;
                  this.logs.push({
                    time: this.get_curr_time(),
                    status: 'успех',
                    message: `Конвертер #${item.id} статус установлен на ${item.status}`,
                  });
                }
              }
            }
          });
      } catch (error) {
        this.logs.push({
          time: this.get_curr_time(),
          status: 'ошибка',
          message: 'Ошибка при обновлении конвертора',
        });
      }
    }, 100);

    setInterval(this.scrollLogsToBottom, 100);
    setInterval(this.scrollLogsToBottom1, 100);
  },
  beforeUnmount() {
    this.stop_cl_generation();
  },
};
</script>

<style>
body, html {
  margin: 0;
  padding: 0;
  height: 100%;
  overflow: hidden; /* Отключаем прокрутку страницы */
}

#app {
  display: flex;
  flex-direction: column;
  height: 100vh;
  padding: 20px;
  font-family: Avenir, Helvetica, Arial, sans-serif;
  box-sizing: border-box;
}

.container {
  display: flex;
  gap: 20px;
  flex: 1;
  overflow: hidden; /* Отключаем прокрутку внутри контейнера */
}

.column {
  border: 1px solid #ddd;
  border-radius: 8px;
  padding: 20px;
  background-color: #f9f9f9;
  overflow-y: auto; /* Включаем прокрутку внутри колонок, если контента много */
}

.management {
  width: 300px; /* Фиксированная ширина */
  flex: 0 0 auto; /* Запрет на растягивание */
  display: flex;
  flex-direction: column;
}

.analytics {
  flex: 1; /* Занимает всё оставшееся пространство */
}

.controls {
  margin-top: auto; /* Кнопки внизу */
  display: flex;
  gap: 10px;
}

.controls button {
  padding: 10px 20px;
  font-size: 16px;
  cursor: pointer;
  border: none;
  border-radius: 4px;
  background-color: #42b983;
  color: white;
  flex: 1; /* Кнопки занимают равное пространство */
}

.controls button:disabled {
  background-color: #ccc;
  cursor: not-allowed;
}

.settings {
  margin-bottom: 20px;
}

.setting {
  margin-bottom: 15px;
}

.setting label {
  display: block;
  margin-bottom: 5px;
  font-weight: bold;
}

.setting input[type="number"] {
  width: 100px;
  padding: 8px;
  font-size: 16px;
  border: 1px solid #ddd;
  border-radius: 4px;
  margin-right: 10px;
}

.file-size-range {
  display: flex;
  align-items: center;
  gap: 10px; /* Расстояние между элементами */
}

.file-size-range input {
  width: 80px; /* Ширина полей ввода */
  padding: 8px;
  font-size: 16px;
  border: 1px solid #ddd;
  border-radius: 4px;
}

.status-text {
  font-weight: bold;
}

.status-text.running {
  color: #2e7d32; /* Зелёный цвет для статуса "Запущен" */
}

.status-text.stopped {
  color: #c62828; /* Красный цвет для статуса "Остановлен" */
}

.converter-list,
.client-list {
  display: flex;
  flex-wrap: wrap; /* Перенос карточек на новую строку, если не хватает места */
  gap: 10px; /* Расстояние между карточками */
}

.converter-card,
.client-card {
  border: 1px solid #ddd;
  border-radius: 8px;
  padding: 16px;
  width: 200px;
  box-shadow: 0 2px 4px rgba(0, 0, 0, 0.1);
}

.logs {
  position: sticky;
  bottom: 0;
  background-color: #fff;
  border: 1px solid #ddd; /* Рамка */
  border-radius: 8px; /* Скругление углов */
  padding: 20px;
  z-index: 100;
  margin-top: 20px; /* Отступ сверху */
}

.log-list {
  max-height: 150px;
  overflow-y: auto;
  border: 1px solid #ddd;
  border-radius: 4px;
  padding: 10px;
  background-color: #f9f9f9;
}

.log-item {
  padding: 5px;
  border-bottom: 1px solid #eee;
  display: flex;
  gap: 10px;
  align-items: center;
}

.log-item:last-child {
  border-bottom: none;
}

.log-time {
  font-weight: bold;
  color: #666;
}

.log-status {
  padding: 2px 6px;
  border-radius: 4px;
  font-size: 12px;
  font-weight: bold;
  text-transform: uppercase;
}

.log-status.успех {
  background-color: #e8f5e9;
  color: #2e7d32;
}

.log-status.ошибка {
  background-color: #ffebee;
  color: #c62828;
}

.log-status.предупреждение {
  background-color: #fff3e0;
  color: #ef6c00;
}

.log-message {
  flex: 1;
}
</style>