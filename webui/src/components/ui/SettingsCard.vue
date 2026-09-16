<template>
  <div 
    class="settings-card flex items-center gap-4 px-5 py-4 transition-all duration-150 active:scale-[0.99] cursor-pointer select-none bg-surface-container-low hover:bg-surface-container"
    :class="shapeClass"
    @click="handleClick"
  >
    <!-- Icon Container (40px x 40px) -->
    <div class="icon-wrap w-10 h-10 rounded-xl bg-surface-container-high text-primary flex items-center justify-center shrink-0 [&>svg]:w-5 [&>svg]:h-5">
      <slot name="icon" />
    </div>

    <!-- Content & Typography -->
    <div class="content-wrap flex-1 min-w-0">
      <h3 class="label text-sm font-medium text-on-surface truncate leading-tight">
        {{ title }}
      </h3>
      <p v-if="subtitle" class="subtitle text-xs text-on-surface-variant mt-1 line-clamp-2">
        {{ subtitle }}
      </p>
    </div>

    <!-- Action / Chevron Slot -->
    <div v-if="$slots.action" class="action-wrap shrink-0 text-on-surface-variant">
      <slot name="action" />
    </div>
  </div>
</template>

<script setup>
import { computed } from 'vue'

const props = defineProps({
  title: { type: String, required: true },
  subtitle: { type: String, default: '' },
  onClick: { type: Function, default: null },
  // Posisi shape: 'single' | 'first' | 'middle' | 'last'
  position: {
    type: String,
    default: 'single',
    validator: (val) => ['single', 'first', 'middle', 'last'].includes(val)
  }
})

const emit = defineEmits(['click'])

const handleClick = (e) => {
  if (props.onClick) {
    props.onClick(e)
  } else {
    emit('click', e)
  }
}

const shapeClass = computed(() => {
  switch (props.position) {
    case 'first':
      return 'rounded-t-[24px] rounded-b-[6px]'
    case 'middle':
      return 'rounded-[6px]'
    case 'last':
      return 'rounded-t-[6px] rounded-b-[24px]'
    default:
      return 'rounded-[24px]'
  }
})
</script>
